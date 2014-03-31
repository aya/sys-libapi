<?php

class logger
{
	public static function log($message)
	{
		error_log($message);
	}
	
	public static function insert($method, $params, $user, $ip = '')
	{
		$sql = "INSERT INTO user_log (log_method, log_params, log_user, log_date, log_ip) VALUES ('{$method}', '".json_encode($params)."', '{$user}', UNIX_TIMESTAMP(), '{$ip}')");
		$GLOBALS['db']->query($sql, mysql::NO_ROW);
	}
}

?>
