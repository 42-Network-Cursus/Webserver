<?php
    print("<!DOCTYPE html>");
    print("<html>");
    print("<head>");
    print("<title>CGI test page</title>");
    print("</head>");
    print("<body>");
    print("<h1>CGI test page</h1>");
    print("<p>");
    print($_POST["answer"]);
    print("</p>");
    print("</body>");
    print("</html>");
?>