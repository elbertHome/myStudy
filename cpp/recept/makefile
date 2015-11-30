CC=g++
CFLAGS=-Wall -g -I/usr/include/mysql
LDFLAGS=-lcurl -lexpat -lz -lm -lssl -lmysqlpp
OBJS=zn_xml.o zn_untar.o zn_stream.o zn_misc.o zn_mailbox_dump.o zn_mailbox.o zn_http.o zn_mailfile.o zn_metafile.o  zn_cimap.o zn_gunzip.o zn_message_digest.o zn_logger.o zn_process_matcher.o zn_config_manager.o zn_ldap.o zn_migration_queue.o zn_zimbra_soap.o zn_hostlist.o zn_untar_utils.o zn_folder_info.o zn_utf7_convert.o
DEPS=zn_exception.hpp zn_misc.cpp zn_misc.hpp zn_retriable.hpp zn_exclusive_lock.hpp

all: md_msgmove

md_msgmove: zn_main.cpp zn_network_req_generator.hpp $(DEPS) $(OBJS)
	$(CC) $(CFLAGS) zn_main.cpp $(OBJS) -o md_msgmove $(LDFLAGS)

zn_xml.o: zn_xml.cpp zn_xml.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_xml.cpp -c

zn_untar.o: zn_untar.cpp zn_untar.hpp zn_tar_internal.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_untar.cpp -c

zn_stream.o: zn_stream.cpp zn_stream.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_stream.cpp -c

zn_misc.o: zn_misc.cpp zn_misc.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_misc.cpp -c

zn_mailbox_dump.o: zn_mailbox_dump.cpp zn_mailbox_dump.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_mailbox_dump.cpp -c

zn_mailbox.o: zn_mailbox.cpp zn_mailbox.hpp zn_filterable_list.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_mailbox.cpp -c

zn_http.o: zn_http.cpp zn_http.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_http.cpp -c

zn_mailfile.o: zn_mailfile.cpp zn_mailfile.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_mailfile.cpp -c

zn_metafile.o: zn_metafile.cpp zn_metafile.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_metafile.cpp -c

zn_cimap.o: zn_cimap.cpp zn_cimap.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_cimap.cpp -c

zn_gunzip.o: zn_gunzip.cpp zn_gunzip.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_gunzip.cpp -c

zn_message_digest.o: zn_message_digest.cpp zn_metafile.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_message_digest.cpp -c

zn_logger.o: zn_logger.cpp zn_logger.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_logger.cpp -c

zn_process_matcher.o: zn_process_matcher.cpp zn_process_matcher.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_process_matcher.cpp -c

zn_config_manager.o: zn_config_manager.cpp zn_config_manager.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_config_manager.cpp -c

zn_ldap.o: zn_ldap.cpp zn_ldap.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_ldap.cpp -c

zn_migration_queue.o: zn_migration_queue.cpp zn_migration_queue.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_migration_queue.cpp -c

zn_zimbra_soap.o: zn_zimbra_soap.cpp zn_zimbra_soap.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_zimbra_soap.cpp -c

zn_hostlist.o: zn_hostlist.cpp zn_hostlist.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_hostlist.cpp -c

zn_untar_utils.o: zn_untar_utils.cpp zn_untar_utils.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_untar_utils.cpp -c

zn_folder_info.o: zn_folder_info.cpp zn_folder_info.hpp zn_filterable_list.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_folder_info.cpp -c

zn_utf7_convert.o: zn_utf7_convert.cpp zn_utf7_convert.hpp $(DEPS)
	$(CC) $(CFLAGS) zn_utf7_convert.cpp -c

clean:
	rm -f *.o
	rm -f md_msgmove
