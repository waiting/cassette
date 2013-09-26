CREATE TABLE am_account_cates (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, 
  name CHAR(64) NOT NULL, 
  desc CHAR(128) NOT NULL, 
  type CHAR(16) NOT NULL CONSTRAINT type REFERENCES am_account_types(name) ON UPDATE CASCADE, 
  url CHAR(512) NOT NULL, 
  icon VARCHAR(128) NOT NULL, 
  startup CHAR(16) NOT NULL DEFAULT ('website'), 
  keywords CHAR(128) NOT NULL, 
  time INT NOT NULL DEFAULT (0));
CREATE TABLE am_account_types (
  name CHAR(16) NOT NULL, 
  rank INT NOT NULL DEFAULT (0), 
  CONSTRAINT sqlite_autoindex_am_account_types_1 PRIMARY KEY (name));
CREATE TABLE am_accounts (
  myname CHAR(64) NOT NULL, 
  account_name BLOB(32) NOT NULL, 
  account_pwd BLOB(32) NOT NULL, 
  cate INT NOT NULL CONSTRAINT cate REFERENCES am_account_cates(id) ON DELETE RESTRICT ON UPDATE CASCADE DEFAULT (0), 
  user INT NOT NULL CONSTRAINT user REFERENCES am_users(id) ON DELETE RESTRICT ON UPDATE CASCADE DEFAULT (0), 
  safe_rank INT NOT NULL DEFAULT (0), 
  comment CHAR(128) NOT NULL, 
  time INT NOT NULL DEFAULT (0), 
  CONSTRAINT sqlite_autoindex_am_accounts_1 PRIMARY KEY (user, myname));
CREATE TABLE am_browsers (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, 
  name CHAR(32), 
  title CHAR(32), 
  exe_name CHAR(32));
CREATE TABLE am_secret_keys (
  key CHAR(64) NOT NULL, 
  type CHAR(32) NOT NULL, 
  username CHAR(32) NOT NULL, 
  invalid_time INT NOT NULL, 
  valid_count INT NOT NULL, 
  time INT NOT NULL, 
  CONSTRAINT sqlite_autoindex_am_secret_keys_1 PRIMARY KEY (key));
CREATE TABLE am_users (
  id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, 
  name CHAR(32) NOT NULL, 
  pwd BLOB(32) NOT NULL, 
  protect INT NOT NULL DEFAULT (0), 
  condone INT NOT NULL DEFAULT (0), 
  cur_condone INT NOT NULL DEFAULT (0), 
  unlock_time INT NOT NULL DEFAULT (0), 
  hotkey INT NOT NULL DEFAULT (0), 
  time INT NOT NULL DEFAULT (0));
CREATE UNIQUE INDEX name ON am_users (name);
INSERT INTO am_account_types( name, rank ) VALUES ( '��ͨ', 20 );
INSERT INTO am_account_types( name, rank ) VALUES ( '��Ϸ', 20 );
INSERT INTO am_account_types( name, rank ) VALUES ( '����', 20 );
INSERT INTO am_account_types( name, rank ) VALUES ( '����', 20 );
INSERT INTO am_account_types( name, rank ) VALUES ( '��Ƶ', 20 );
INSERT INTO am_account_types( name, rank ) VALUES ( '�罻', 30 );
INSERT INTO am_account_types( name, rank ) VALUES ( '����', 30 );
INSERT INTO am_account_types( name, rank ) VALUES ( 'ҽ��', 40 );
INSERT INTO am_account_types( name, rank ) VALUES ( '����', 40 );
INSERT INTO am_account_types( name, rank ) VALUES ( '����', 50 );
INSERT INTO am_account_types( name, rank ) VALUES ( '����', 70 );
INSERT INTO am_account_types( name, rank ) VALUES ( '����', 100 );
INSERT INTO am_users ( id, name, pwd, protect, condone, cur_condone, unlock_time, hotkey, time )
VALUES ( -1, 'root', '', 2, 0, 0, 0, 0, 0 );
INSERT INTO am_browsers ( id, name, title, exe_name )
VALUES ( 1, 'IE', 'Windows Internet Explorer', 'iexplore.exe' );
INSERT INTO am_browsers ( id, name, title, exe_name )
VALUES ( 2, 'IE*', 'Internet Explorer', 'iexplorer.exe' );
INSERT INTO am_browsers ( id, name, title, exe_name )
VALUES ( 3, 'Chrome', 'Google Chrome', 'chrome.exe' );
INSERT INTO am_browsers ( id, name, title, exe_name )
VALUES ( 4, 'Firefox', 'Mozilla Firefox', 'firefox.exe' );
INSERT INTO am_browsers ( id, name, title, exe_name )
VALUES ( 5, 'Opera', 'Opera', 'opera.exe' );
