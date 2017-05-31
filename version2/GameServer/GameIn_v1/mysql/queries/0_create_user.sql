DROP USER IF EXISTS 'test'@'localhost';

CREATE USER 'test'@'localhost' IDENTIFIED WITH sha256_password BY '1qaz2wsx3edc4rfv' PASSWORD EXPIRE NEVER;

GRANT ALL ON *.* TO 'test'@'localhost';

