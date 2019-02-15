/*******
********************轴角********************
*******************欧拉角*******************
******************旋转矩阵******************
*******************四元数*******************
http://www.j3d.org/matrix_faq/matrfaq_latest.html#Q60
https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
https://www.cnblogs.com/marty86/p/7868223.html
********/
//create by Moujiker
//date: 15.2.2019

#include <cmath>

enum kRotSequence
{ 
	zyx, 
	zyz, 
	zxy, 
	zxz, 
	yxz, 
	yxy, 
	yzx, 
	yzy, 
	xyz, 
	xyx, 
	xzy, 
	xzx
};

class AxisAngle;

class Quaternion 
{
public:
	Quaternion() : x(0), y(0), z(0), w(1){}
	Quaternion(double x_, double y_, double z_, double w_) :x(x_), y(y_), z(z_), w(w_){}

	void Normalize()
	{ 
		double sum = sqrt(x*x + y*y + z*z + w*w);
		x /= sum;
		y /= sum;
		z /= sum;
		w /= sum;
	}
    double w() { return w; }
    double x() { return x; }
    double y() { return y; }
    double z() { return z; }

    void Set_WXYZ(double w_, double x_, double y_, double z_)
    {
        w = w_;
        x = x_;
        y = y_;
        z = z_;
    }

    void conjugate();
    void inverse();
    void magnitude();
    void multiply_two_quaternion();
	void FromAxisAngle();

    static void toEulerAngle(const Quaternion& q, double& roll, double& pitch, double& yaw)
    {
        // roll (x-axis rotation)
        double sinr_cosp = +2.0 * (q.w() * q.x() + q.y() * q.z());
        double cosr_cosp = +1.0 - 2.0 * (q.x() * q.x() + q.y() * q.y());
        roll = atan2(sinr_cosp, cosr_cosp);

        // pitch (y-axis rotation)
        double sinp = +2.0 * (q.w() * q.y() - q.z() * q.x());
        if (fabs(sinp) >= 1)
            pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
        else
            pitch = asin(sinp);

        // yaw (z-axis rotation)
        double siny_cosp = +2.0 * (q.w() * q.z() + q.x() * q.y());
        double cosy_cosp = +1.0 - 2.0 * (q.y() * q.y() + q.z() * q.z());
        yaw = atan2(siny_cosp, cosy_cosp);
    }
private:
	double x, y, z, w;
};
  
class AxisAngle
{
public:
	AxisAngle(){}

	void FromQuaternion()
	{
	}

	Quaternion ToQuaternion()
	{
		Normalize();
		return Quaternion(x* sin(angle / 2.0), y * sin(angle / 2.0), z * sin(angle / 2.0), cos(angle / 2.0));
	}

	void Normalize()
	{
		double sum = sqrt(x * x + y * y + z * z);
		x /= sum;
		y /= sum;
		z /= sum;
	}

private:
	double x, y, z, angle;
};

class EulerAngle
{
public:
	EulerAngle(){}

    Quaternion toQuaternion(double yaw, double pitch, double roll) // yaw (Z), pitch (Y), roll (X)
    {
        // Abbreviations for the various angular functions
        double cy = cos(yaw * 0.5);
        double sy = sin(yaw * 0.5);
        double cp = cos(pitch * 0.5);
        double sp = sin(pitch * 0.5);
        double cr = cos(roll * 0.5);
        double sr = sin(roll * 0.5);

        double w = cy * cp * cr + sy * sp * sr;
        double x = cy * cp * sr - sy * sp * cr;
        double y = sy * cp * sr + cy * sp * cr;
        double z = sy * cp * cr - cy * sp * sr;
        Quaternion q(x,y,z,w);
        return q;
    }

private:
    //
	double yaw, pitch, roll;
};


class RotateMatrix
{

    void FromEulerAngle(double angle_x, double angle_y, double angle_z)
    {
 
        | CE          - CF          D     0 |
    M = | BDE + AF    - BDF + AE    - BC  0 |
        | -ADE + BF   ADF + BE      AC    0 |
        | 0           0             0     1 |
 

        A = cos(angle_x);
        B = sin(angle_x);
        C = cos(angle_y);
        D = sin(angle_y);
        E = cos(angle_z);
        F = sin(angle_z);
        AD = A * D;
        BD = B * D;
        mat[0] = C * E;
        mat[1] = -C * F;
        mat[2] = D;

        mat[4] = BD * E + A * F;
        mat[5] = -BD * F + A * E;
        mat[6] = -B * C;
        mat[8] = -AD * E + B * F;
        mat[9] = AD * F + B * E;
        mat[10] = A * C;
        mat[3] = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
        mat[15] = 1;
    }


    void toEulerAngle()
    {
        angle_y = D = asin(mat[2]);        /* Calculate Y-axis angle */
        C = cos(angle_y);
        angle_y *= RADIANS;
        if (fabs(C) > 0.005)             /* Gimball lock? */
        {
            trx = mat[10] / C;           /* No, so get X-axis angle */
            try = -mat[6] / C;
            angle_x = atan2(try, trx) * RADIANS;
            trx = mat[0] / C;            /* Get Z-axis angle */
            try = -mat[1] / C;
            angle_z = atan2(try, trx) * RADIANS;
        }
        else                                 /* Gimball lock has occurred */
        {
            angle_x = 0;                      /* Set X-axis angle to zero */
            trx = mat[5];                 /* And calculate Z-axis angle */
            try = mat[4];
            angle_z = atan2(try, trx) * RADIANS;
        }

        /* return only positive angles in [0,360] */
        if (angle_x < 0) angle_x += 360;
        if (angle_y < 0) angle_y += 360;
        if (angle_z < 0) angle_z += 360;
        ---------------------------------- -
    }


    void fromQuantern()
    {

        //The following code snipped does most of the work(phi being the rotation
        //    angle and (u, v, w) the rotation(axis) vector) :

        rcos = cos(phi);
        rsin = sin(phi);
        rotMat[0][0] = rcos + u*u*(1 - rcos);
        rotMat[1][0] = w * rsin + v*u*(1 - rcos);
        rotMat[2][0] = -v * rsin + w*u*(1 - rcos);
        rotMat[0][1] = -w * rsin + u*v*(1 - rcos);
        rotMat[1][1] = rcos + v*v*(1 - rcos);
        rotMat[2][1] = u * rsin + w*v*(1 - rcos);
        rotMat[0][2] = v * rsin + u*w*(1 - rcos);
        rotMat[1][2] = -u * rsin + v*w*(1 - rcos);
        rotMat[2][2] = rcos + w*w*(1 - rcos);

      //Don't forget to set the rest of the matrix to 0 (1 at [3][3]) if
      //you are using 4x4 matrices!
    }

private:
    double rotMat[4][4];
};


//test
int main()
{
}