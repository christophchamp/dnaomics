<?php
$headers = "From: myplace@example.com\r\n";
$headers .= "Reply-To: myplace2@example.com\r\n";
$headers .= "Return-Path: myplace@example.com\r\n";
$headers .= "CC: sombodyelse@example.com\r\n";
$headers .= "BCC: hidden@example.com\r\n";

if ( mail($to,$subject,$message,$headers) ) {
    echo "The email has been sent!";
} else {
    echo "The email has failed!";
}
?>
