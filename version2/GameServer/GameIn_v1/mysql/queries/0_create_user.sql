DROP USER IF EXISTS 'test'@'localhost';

CREATE USER 'test'@'localhost' IDENTIFIED BY '1qaz2wsx3edc4rfv' PASSWORD EXPIRE NEVER;

GRANT ALL ON *.* TO 'test'@'localhost';

