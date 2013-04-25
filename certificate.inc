<?php

class certificate
{
	public static function Decrypt($message)
	{
		putenv('GNUPGHOME=/dns/com/bus-it/.gnupg');
		$res = gnupg_init();
		gnupg_adddecryptkey($res,$GLOBALS['CONFIG']['GPG_KEY'],self::$PRIVATE);

		if( is_array($message['content']) )
		{
			$clear = gnupg_decrypt($res,$message['content']['data']);
			$message['content']['data'] = $clear;
		}
		elseif( strlen($message['data']) > 1 )
		{
			$clear = gnupg_decrypt($res,$message['data']);
			$message['content']['data'] = $clear;
		}
		else
		{
			$clear = gnupg_decrypt($res,$message);
			$message = $clear;
		}
		
		return $message;
	}
	
	public static function Encrypt($message)
	{
		putenv('GNUPGHOME=/dns/com/bus-it/.gnupg');
		$res = gnupg_init();
		gnupg_addencryptkey($res,$GLOBALS['CONFIG']['GPG_KEY']);
		
		if( strlen($message['data']) > 1 )
		{
			$enc = gnupg_encrypt($res,$message['data']);
			$message = $enc;
		}
		else
		{
			$enc = gnupg_encrypt($res,$message);
			$message = $enc;
		}
		
		return $message;
	}
	
	public static function Verify($message, $finger)
	{
		putenv('GNUPGHOME=/dns/com/bus-it/.gnupg');
		$res = gnupg_init();
		
		$clear = '';
		if( strlen($message['data']) > 1 )
		{
			$info = gnupg_verify($res,$message['data'],false,$clear);
			$message['data'] = $clear;
		}
		else
		{
			$info = gnupg_verify($res,$message,false,$clear);
			$message = $clear;
		}

		if( $finger && $info[0]['fingerprint'] == $finger )
			return $message;
		else
			return false;
	}
	
	private function DecryptVerify($message, $finger)
	{	
		putenv('GNUPGHOME=/dns/com/bus-it/.gnupg');
		$res = gnupg_init();
		gnupg_adddecryptkey($res,$GLOBALS['CONFIG']['GPG_KEY'],self::$PRIVATE);

		$clear = '';
		if( strlen($message['data']) > 1 )
			$info = gnupg_decryptverify($res,$message['data'],$clear);
		else
			$info = gnupg_decryptverify($res,$message,$clear);

		if( $finger && $info[0]['fingerprint'] == $finger )
			return $message;
		else
			return false;
	}
	
	private function Sign($message)
	{
		// TODO
		return $message;
	}
	
	private function EncryptSign($message)
	{
		// TODO
		return $message;
	}
}

?>
