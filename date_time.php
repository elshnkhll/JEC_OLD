<?php
	
	date_default_timezone_set( $_GET['tz'] );
	echo date("y;m;d;N;G;i;s;");
	exit;
