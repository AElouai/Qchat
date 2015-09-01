CREATE DATABASE chat;

CREATE TABLE message (
  `src` char(50) NOT NULL,
  `dst` char(50) NOT NULL,
  `ordate` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `msg` char(200) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ;
