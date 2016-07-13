import mysql.connector

cnx = mysql.connector.connect(user="batch_bizdb",
                              password="bizdb",
                              host="localhost",
                              database="business_db")

cursor = cnx.cursor()
cursor.execute("select id_type, record_update_program_id, record_update_time from id_allocation")

for (id_type, p_id, update_time) in cursor:
    print "id_type = %s \np_id = %s\nupdate_time = %s\n" % (id_type, p_id, update_time)

cursor.close()
cnx.close()





