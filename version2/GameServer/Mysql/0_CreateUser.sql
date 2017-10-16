CREATE USER 'development'@'localhost'
  IDENTIFIED WITH sha256_password BY '1qaz2wsx3edc4rfv'
  PASSWORD EXPIRE NEVER;

GRANT ALL ON *.* TO 'development'@'localhost';

Create Database 'GameIn';