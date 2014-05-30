<?php

// =================================
// CROSSDOMAIN
// =================================
if( preg_match("/^\\/?crossdomain.xml$/i", $_GET['__rewrite']) )
{
	header("Content-Type: text/xml");
	echo "<?xml version=\"1.0\"?><cross-domain-policy><allow-access-from domain=\"*\"/><site-control permitted-cross-domain-policies=\"all\"/><allow-http-request-headers-from domain=\"*\" headers=\"*\"/></cross-domain-policy>";
	exit;
}

// =================================
// DEFAULT PARAMETERS (OVERRIDE IN SITE CONFIG)
// =================================
$GLOBALS['CONFIG'] 					= array();
$GLOBALS['CONFIG']['CLASSPATH'] 	= __DIR__ . '/';
$GLOBALS['CONFIG']['ROOT'] 			= dirname($GLOBALS['CONFIG']['CLASSPATH']) . '/';
$GLOBALS['CONFIG']['APIDIR'] 		= $GLOBALS['CONFIG']['ROOT'] . 'api/';
$GLOBALS['CONFIG']['DEBUG'] 		= true;
$GLOBALS['CONFIG']['THROW_ERRORS'] 	= (E_ALL | E_STRICT) & ~E_NOTICE;


switch( $_SERVER["HTTP_HOST"] )
{
	case 'api.dev.anotherservice.com':
		$conf = parse_ini_file('/dns/com/anotherservice/etc/settings/api.dev.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.anotherservice.com':
		$conf = parse_ini_file('/dns/com/anotherservice/etc/settings/api.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.dev.bus-it.com':
		$conf = parse_ini_file('/dns/com/bus-it/etc/settings/api.dev.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.bus-it.com':
		$conf = parse_ini_file('/dns/com/bus-it/etc/settings/api.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.dev.olympe.in':
		$conf = parse_ini_file('/dns/in/olympe/etc/settings/api.dev.ini', true);
		$conf['type'] = 'file';
	break;
	case 'api.olympe.in':
		$conf = parse_ini_file('/dns/in/olympe/etc/settings/api.ini', true);
		$conf['type'] = 'file';
	break;
	default:
	    $conf = parse_ini_file('/dns/com/anotherservice/etc/settings/api.ini', true);
	    $conf['type'] = 'file';
}

if( $conf['type'] == 'file' && isset($conf['section']) )
{
	foreach( $conf[$conf['section']] as $key => $value )
		$GLOBALS['CONFIG'][$key] = $value;
}

unset($conf);

$GLOBALS['CONFIG']['PARAMETERS'] 			= array();
$GLOBALS['CONFIG']['PARAMETERS']['FORMAT']	= array('f','format','output','outputformat','o');
$GLOBALS['CONFIG']['PARAMETERS']['ACTION']	= array('action');
$GLOBALS['CONFIG']['PARAMETERS']['NOHTTP']	= array('nohttperrorheader');
$GLOBALS['CONFIG']['PARAMETERS']['XML']		= array('xml','x');
$GLOBALS['CONFIG']['PARAMETERS']['JSON']	= array('json','j');
$GLOBALS['CONFIG']['PARAMETERS']['TOKEN']	= array('secret','auth','creditentials');

define('CONFIG_LOADED', true);

?>
