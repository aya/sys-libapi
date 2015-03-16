<?php

class logger
{
	public static function log($message)
	{
		error_log($message);
	}
	
	public static function benchmark($log)
	{
		@file_put_contents($GLOBALS['CONFIG']['BENCHMARK_LOG_DIR'].'/'.$_SERVER["HTTP_HOST"].'.log', json_encode($log)."\n", FILE_APPEND | LOCK_EX);
	}
	
	public static function insert($method, $params, $user)
	{
		$ip = $_SERVER['HTTP_ORIGINAL_IP'];
		$sql = "INSERT INTO user_log (log_method, log_params, log_user, log_date, log_ip) VALUES ('{$method}', '".json_encode($params)."', '{$user}', UNIX_TIMESTAMP(), '{$ip}')";
		$GLOBALS['db']->query($sql, mysql::NO_ROW);
	}
}

?>