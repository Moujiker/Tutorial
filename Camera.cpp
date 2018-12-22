//相机参数

#include <Eigen/Core>

class Camera
{

public:
	Camera();
	~Camera();

private:


	 double m_focal;         /* Focal length */
	 double m_k[2];          /* Distortion parameters */
	 double m_R[9], m_t[3];  /* Extrinsics */
	 int m_width, m_height;  /* Image dimensions */

	 double m_Pmatrix[12];

	 FundamentalMatrix m7;
	 FundamentalMatrix m8;
}