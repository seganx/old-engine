-- DROP TABLE IF EXISTS authen;

CREATE TABLE IF NOT EXISTS authen
(
	profile_id		INT(10) UNSIGNED AUTO_INCREMENT PRIMARY KEY,
	device_id		VARCHAR(50) NOT NULL,
	last_device_id	VARCHAR(50) DEFAULT NULL,
	google_id 		VARCHAR(50) DEFAULT NULL,
	facebook_id	 	VARCHAR(50) DEFAULT NULL,
	username		VARCHAR(50) DEFAULT NULL,
	password		VARCHAR(50) DEFAULT NULL,
    register_date 	TIMESTAMP	
);