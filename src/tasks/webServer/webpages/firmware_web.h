/*
 * firmware_web.h
 *
 *  Created on: 7 déc. 2023
 *      Author: cyp
 */

#ifndef FIRMWARE_WEB_H_
#define FIRMWARE_WEB_H_




/* Get: Network Information: function NetinfoCallback(o), getNetinfo() */
#define firmware_js	"function FwVersionCallback(o){"\
									 "$('txtversion').value=o.version;"\
									 "if(typeof(window.external)!='undefined'){"\
										"obj=$$_ie('input','version');"\
									 "}else{"\
										"obj=$$('version');"\
									 "}"\
								"}"\
								" "\
								"function getFwVersion(){"\
									 "var oUpdate;"\
									 "setTimeout(function(){"\
									 	"oUpdate=new AJAX('get_fwversion.cgi',function(t){"\
									 		"try{eval(t);}catch(e){alert(e);}"\
									 "});"\
									 "oUpdate.doGet();},300);"\
								 "}"

#define firmware_page 		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>SM Firmware</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'"\
								"</head>"\
								"<script type='text/javascript' src='ajax.js'></script>"\
								"<script type='text/javascript' src='firmware.js'></script>"\
								"<body onload='getFwVersion();'>"\
									"<div>"\
										"SM Gateway Firmware Update"\
									"</div>"\
									"<ul>"\
										"<li><label for='txtversion'>VER:</label><input id='txtversion' name='version' type='text' size='20' disabled='disabled'/></li> "\
									"</ul>"\
									"<form id='form' action='fwupload.html' method='post' enctype='multipart/form-data'>"\
										"<input id='file' name='file' type='file'/>"\
										"<br>"\
										"<button>Upload</button>"\
									"</form>"\
								"</body>"\
							"</html>"

#define fwupload_page 		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>SM Firmware</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'"\
								"</head>"\
								"<body>"\
									"<div>"\
										"SM Gateway Firmware Upload Successfull !"\
									"</div>"\
									"<div>"\
										"Device will now restart. Goodbye !"\
									"</div>"\
								"</body>"\
							"</html>"

#define fwfailed_page 		"<!DOCTYPE html>"\
							"<html>"\
								"<head>"\
								"<title>SM Firmware</title>"\
								"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'"\
								"</head>"\
								"<body>"\
									"<div>"\
										"SM Gateway Firmware Upload Failed ! :c"\
									"</div>"\
									"<div>"\
										"We recommend you try again, but the firmware you have might be corrupted."\
									"</div>"\
								"</body>"\
							"</html>"

#endif /* FIRMWARE_WEB_H_ */
