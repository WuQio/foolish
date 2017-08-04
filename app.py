#!/usr/bin/python
# coding=utf-8
from flask import Flask

app = Flask(__name__)

@app.route('/')
return '<h1>Foolish</h1>'

if __name__ == '__main__':
	app.run(debug=True)
