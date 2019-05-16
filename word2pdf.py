#-*- coding:utf-8 -*-
# doc2pdf.py: python script to convert doc to pdf with bookmarks!
# Requires Office 2007 SP2
# Requires python for win32 extension
'''
参考:http://blog.csdn.net/rumswell/article/details/7434302
'''
import sys, os
#import win32api
#from win32com.client import Dispatch, constants, gencache
#from config import REPORT_DOC_PATH,REPORT_PDF_PATH

from win32com import client


    
def word2pdf(filename, outputFile):

  try:
    print("enter read doc")
    word = client.DispatchEx("Word.Application")
    worddoc = word.Documents.Open(filename,ReadOnly = 1)
    print("read finish")
    worddoc.SaveAs(outputFile, FileFormat = 17)
    worddoc.Close()
    print("finish transform")
    return pdf_name
  except:
    print(' exception')
    return -1

if __name__=='__main__':
  output="C:\\Users\\zhaobaolong\\Desktop\\A6\\abc.pdf"
  intput="C:\\Users\\zhaobaolong\\Desktop\\A6\\abc.docx"
  rc = word2pdf(intput, output)
  if rc:
    sys.exit(rc)
  sys.exit(0)
