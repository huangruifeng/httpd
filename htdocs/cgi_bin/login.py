#!/usr/bin/python

import MySQLdb
import os

conn=MySQLdb.connect('localhost','root','',"student_five")
cour=conn.cursor()
query_string =os.environ.get('QUERY_STRING')
str = str(query_string)
str = str+'&'

usr=str[str.find('=')+1:str.find('&')]
str = str[str.find('&')+1:]
	
pawd= str[str.find('=')+1:str.find('&')]
str = str[str.find('&')+1:]


str1 = "select pawd from usrs where id=%s"%(usr)
str3 = "select * from usrs"
cour.execute(str1)
data = cour.fetchone()
data =  "%s"%(data)
cour.execute(str3)
rows = cour.fetchall()
a = cmp(data,pawd)
if a:
	print "<html> <head>login success</head><br>"
	print "<body>"
	for row in rows:
		print row
		print "<br>"

	print' <a href = "html/calculator.html"> '
	print'<center><h3>1.calculator</h3></center></a><br>'
	print '<center><a href = "html/updata.html">'	
	print 'updata</a></center>'
	print "</body></html>"
else:
	print "passwrod error"
conn.commit()
cour.close()
conn.close()





