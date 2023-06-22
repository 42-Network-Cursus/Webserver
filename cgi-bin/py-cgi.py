#!/usr/bin/env python

import cgi

# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from the submitted form
# name = form.getvalue('name', 'Anonymous')

# Set the content type to HTML
print('Content-Type: text/html\n')

# Generate the HTML response
print('<html>')
print('<head>')
print('<title>Python CGI Example</title>')
print('</head>')
print('<body>')
# print('<h1>Hello, {}!</h1>'.format(name))
print('</body>')
print('</html>')