-- DROP TABLE IF EXISTS shop;

CREATE TABLE IF NOT EXISTS shop
(
	id						INT UNSIGNED NOT NULL UNIQUE PRIMARY KEY,
	platform				TINYINT UNSIGNED DEFAULT 0, -- googleplay, cafebazaar, mayket, etc
	group					TINYINT UNSIGNED DEFAULT 0, -- treasure, decorations, etc
	style					TINYINT UNSIGNED DEFAULT 0, -- ordinary, special, popup, etc
	active					BOOLEAN DEFAULT FALSE,
			
	title					VARCHAR(80) DEFAULT NULL,
	label					VARCHAR(100) DEFAULT NULL,
	desc					VARCHAR(200) DEFAULT NULL,
			
	
	price_main				INT UNSIGNED DEFAULT 0,
	price_off				INT UNSIGNED DEFAULT 0,
	price_off_percent		TINYINT UNSIGNED DEFAULT 0,
	price_currency			TINYINT UNSIGNED DEFAULT 0,
	
	timer_active
	timer_inactive
	
);