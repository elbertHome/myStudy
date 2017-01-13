DROP DATABASE IF EXISTS `mailDB`;
CREATE DATABASE `mailDB` DEFAULT CHARSET=utf8mb4;


DROP TABLE IF EXISTS  `mailDB`.`authTbl`;
CREATE TABLE `mailDB`.`authTbl` (
	`user_name` varchar(64) NOT NULL,
	`user_pwd` varchar(16) NOT NULL,
	`user_login_id` varchar(64) NOT NULL,
	`fixed_login_pwd` varchar(16) NOT NULL,
	`imap_serv_ip` varchar(16) NOT NULL, 
	PRIMARY KEY (`user_name`),
	KEY `authTbl_idx_1` (`user_name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
