-- DROP TABLE IF EXISTS shop;

CREATE TABLE IF NOT EXISTS shop
(
    id                      INT UNSIGNED AUTO_INCREMENT UNIQUE PRIMARY KEY,     -- id of the package in the market
	market_id               INT UNSIGNED DEFAULT 0,     						-- id of the package in the market
    platform                TINYINT UNSIGNED DEFAULT 0, 						-- googleplay, cafebazaar, mayket, etc
    group                   TINYINT UNSIGNED DEFAULT 0, 						-- treasure, decorations, etc
    style                   TINYINT UNSIGNED DEFAULT 0, 						-- ordinary, special, popup, etc
    active                  BOOLEAN DEFAULT FALSE,

    price_main              INT UNSIGNED DEFAULT 0,
    price_off               INT UNSIGNED DEFAULT 0,
    price_off_percent       TINYINT UNSIGNED DEFAULT 0,
    price_currency          TINYINT UNSIGNED DEFAULT 0,
    
    timer_active            DATETIME DEFAULT NULL,
    timer_inactive          DATETIME DEFAULT NULL,

    content                 VARCHAR(1000) DEFAULT NULL  -- application specific content values. can be json ro a simple raw data
);