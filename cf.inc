<?php

class cf
{
	public static function send($uri, $method='GET', $params=array(), $token=false)
	{
		$params = json_encode($params);
		$uri = ltrim(rtrim($uri, '/'), '/');
		$uri = $GLOBALS['CONFIG']['CF_HOST'] . '/' . $uri . '/';
		
		$request = array( 'http' => array( 'user_agent' => 'PHP/5.x (Another Service) API/1.0', 'method' => $method, 'timeout' => 60.0 ));
		$request['http']['content'] = $params;
		$request['http']['header'] = 'Content-Length: ' . strlen($request['http']['content']) . "\n";
		$request['http']['header'] = 'Content-Type: application/json' . "\n";
		$request['http']['header'] = 'Authorization: ' . ($token?"{$token}":"{$GLOBALS['CONFIG']['CF_TOKEN']}") . "\n";

		try
		{
			$fh = fopen($uri, 'r', false, stream_context_create( $request ));
			if( $fh === false )
				throw new ApiException("The upstream API did not respond", 500, "Failed opening API url : " . $uri);

			$response = stream_get_contents($fh);
			fclose($fh);
			return json_decode($response, true);
		}
		catch(Exception $e)
		{
			// get the E_WARNING from the fopen
			$ex = new Exception(preg_replace("/fopen\\((.*?)\\)/", "fopen(-http stream-)", $e->getMessage()), $e->getCode(), $e->getPrevious());
			throw new ApiException("Upstream API failure", 500, "Upstream API failure", $ex);
		}
	}
}

?>