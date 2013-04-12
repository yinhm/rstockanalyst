<?php
session_start();
header("Content-type: text/html; charset=utf-8");

unset($_SESSION['username']);
unset($_SESSION['userid']);
unset($_SESSION['name']);
unset($_SESSION['isadmin']);

?>