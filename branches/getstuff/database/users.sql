create table users(
id int NOT NULL AUTO_INCREMENT,
PRIMARY KEY(id),
name varchar(32) NOT NULL,
username varchar(32),
password varchar(32),
isadmin int
)
