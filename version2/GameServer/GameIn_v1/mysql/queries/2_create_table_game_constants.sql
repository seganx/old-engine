-- DROP TABLE IF EXISTS game_constants;

CREATE TABLE IF NOT EXISTS game_constants
(
	game_id		INT UNSIGNED FOREIGN KEY REFERENCES game_key(id),	-- link to game id
	key			varchar(20) NOT NULL,
	value		varchar(50) DEFAULT NULL,
);