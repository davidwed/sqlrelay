#! /usr/bin/env python

# Copyright (c) 2000 Roman Milner
# See the file COPYING for more information

from SQLRelay import PySQLRClient
import sys, string
import time


def main():
    if len(sys.argv) != 7:
        print "usage:    query.py host port socket user password query"
        sys.exit(0)
    con = PySQLRClient.sqlrconnection(sys.argv[1],int(sys.argv[2]),sys.argv[3],sys.argv[4],sys.argv[5],0,1)
    cur = PySQLRClient.sqlrcursor(con)
    cur.sendQuery(sys.argv[6])
    for row in range(cur.rowCount()):
        num_cols = cur.colCount()
        for col in range(num_cols):
            print '"' + string.strip(cur.getField(row, col)) + '"',
            if col < num_cols-1:
                print ',',
        print 

if __name__ == '__main__':
    main()
