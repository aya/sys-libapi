<?php

class stream
{
	private $error;
	private $socket;
	const EOS = '##EOS##';
	
	public function __construct($server = '127.0.0.1', $port = '1024', $protocol = 'tcp')
	{
		// create the socket
		$this->socket = socket_create(AF_INET, SOCK_STREAM, getprotobyname($protocol));
		
		if( $this->socket === FALSE )
			throw new ApiException('System bot connection failed', 500, 'Server is unavailable');
		
		// establish connection
		$connect = socket_connect($this->socket, $server, $port);
		
		if( $connect === FALSE )
		{
			socket_close($this->socket);
			throw new ApiException('System bot connection failed', 500, 'Server is unavailable');
		}
	}
	
	public function __destruct()
	{
		socket_close($this->socket);
	}
	
	public function send($msg = '', $use_eos = true)
	{
		if( !$this->error )
		{
			// set implicit flush
			ob_implicit_flush();
			
			// indicate end of stream
			if( $use_eos )
				$msg .= self::EOS;
			
			// finish the line
			$msg .= "\r\n";
			
			$send = 0;
			do
			{
				// send the data
				$send += socket_write($this->socket, substr($msg, $send));
				
				if( $send === FALSE )
					throw new ApiException('System bot command failed', 500, 'Unable to execute command');
			}
			while( $send < strlen($msg) );

			return $send;
		}
	}
	
	public function recv($timeout = 5, $interval = 0.05, $delimiter = self::EOS)
	{
		if( !$this->error )
		{
			$result = '';
			$read_complete = false;
			
			for($t = $timeout/$interval; $t > 0; $t--, usleep(1000000 * $interval))
			{
				$read = socket_read($this->socket, 2048, PHP_NORMAL_READ);
				
				if ( $read === FALSE ) // error
					throw new ApiException('System bot command failed', 500, 'Could not read from socket : '.socket_strerror(socket_last_error($this->socket)));
				else if( strlen($read) <= 0 ) // nothing in the stream
					continue;
				else if( strpos($read, $delimiter) !== FALSE )
				{
					// remove the delimiter and convert DOS -> UNIX ending
					$result .= str_replace("\r\n", "\n", substr($read, 0, strpos($read, $delimiter)));
					$read_complete = true;
					break;
				}
				else
					$result .= str_replace("\r\n", "\n", $read); // convert DOS -> UNIX ending
			}
			
			if( !$read_complete ) // timeout !
				throw new ApiException('System bot command timeout', 500, 'Timeout when reading from socket : '.strlen($result).' bytes read');
			
			return $result;
		}
	}
	
	public function wait()
	{
		if( $this->recv() != "##OK##" )
			raise( new StreamException(iSeverity::MINOR, 'Bot result did not succeed') );
	}
}

?>