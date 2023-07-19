<?php
echo("<!DOCTYPE html>");
echo("<html>");
echo("<head>");
echo("<title>CGI test page</title>");
echo("</head>");
echo("<body>");
echo("<h1>CGI test page</h1>");
echo("<p>");
echo(getenv("answer"));
// echo($_GET["answer"]);
echo("</p>");
echo("</body>");
echo("</html>");
?>