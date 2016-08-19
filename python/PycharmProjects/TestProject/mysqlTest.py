import mysql.connector

cnx = mysql.connector.connect(user="batch_bizdb",
                              password="bizdb",
                              host="localhost",
                              database="business")

cursor = cnx.cursor()
strsql = """insert into MSGMOVE_PLAN_DB.MSGMOVE_PLAN_TBL_1 (
                        LOGIN_ID,
                        MAIL_ADDR_EXTRA,
                        MAIL_ADDR,
                        EAS_HOST,
                        NODE_ID,
                        PROC_STATUS,
                        MAIL_MOVE_FLAG,
                        DATA_MOVE_FLAG,
                        INSERT_TIME,
                        MDN,
                        SERVICE_ID)
                        values
                        ('DDCZ0000000100','DDCZ0000000100@ezweb.ne.jp','DDCZ0000000100@ezweb.ne.jp','rzm03box82.ezweb.ne.jp','001',0,1,0,'2016-07-28 09:01:27','',0);"""
cursor.execute("select * from id_allocation")

for vals in cursor:
    print vals
    # print "id_type = %s \np_id = %s\nupdate_time = %s\n" % (id_type, p_id, update_time)

cursor.close()
cnx.close()





