#!/usr/bin/python
import MySQLdb

conn=MySQLdb.connect('localhost','root','',"student_five")
cour=conn.cursor()

#cour.execute("insert into bite values(1,name,age)")
#cour.execute("insert into usr values(1,usr,pawd)")

print "register success"
cour.close()
conn.close()
