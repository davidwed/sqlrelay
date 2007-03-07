import SQLRelay, DateTime
from SQLRelay import PySQLRDB
from Shared.DC.ZRDB.TM import TM

import string, sys
from string import strip, split, find
from time import time

failures=0
calls=0
last_call_time=time()

class DB(TM):

    Database_Error = PySQLRDB.DatabaseError
    Database_Connection=PySQLRDB.connect

    def __init__(self,connection):
        info = string.split(connection)
        if len(info)==4:
            self.host = info[0]
            self.port = int(info[1])
            self.socket = ""
            self.user = info[2]
            self.password = info[3]
        elif len(info)==3:
            self.host = ""
            self.port = 0
            self.socket = info[0]
            self.user = info[1]
            self.password = info[2]
        else:
            raise self.Database_Error, ('Invalid connection string, <code>%s</code>')

    def str(self,v, StringType=type('')):
        if v is None: return ''
        r=str(v)
        if r[-1:]=='L' and type(v) is not StringType: r=r[:-1]
        return r

    def _begin(self):
        self.con = PySQLRDB.connect(self.host, self.port, self.socket, self.user, self.password,0,1)
        self.cur = self.con.cursor()

    def _finish(self, *ignored):
        self.con.commit()
        self.con.close()

    def _abort(self, *ignored):
        self.con.rollback()
        self.con.close()
        
    def tables(self, rdb=0, _care=('TABLE', 'VIEW')):
        r=[]
        a=r.append
        for name, typ in self.db.objects():
            if typ in _care:
                a({'TABLE_NAME': name, 'TABLE_TYPE': typ})
        return r

    def columns(self, table_name):
        try: 
            r=self.cur.execute('select * from %s' % table_name)
        except: 
            return ()
        desc=self.cur.description
        r=[]
        a=r.append
        for name, type, width, ds, p, scale, null_ok in desc:
            if type=='NUMBER' and scale==0: type='INTEGER'
            a({ 'Name': name,
                'Type': type,
                'Precision': p,
                'Scale': scale,
                'Nullable': null_ok,
                })
        return r

    def query(self, query_string, max_rows=9999999):
        global failures, calls, last_call_time
        calls=calls+1
        desc=None
        result=()
        self._register()
        try:
            for qs in filter(None, map(strip,split(query_string, '\0'))):
                r=self.cur.execute(qs)
                if r is None:
                    if desc is not None:
                        if self.cur.description != desc:
                            raise 'Query Error', (
                                'Multiple select schema are not allowed'
                                )
                        if 'append' not in dir(result): 
                            result = list(result)
                        if max_rows:
                            for row in self.cur.fetchmany(max_rows-len(result)):
                                result.append(row)
                    else:
                        desc=self.cur.description
                        if max_rows:
                            if max_rows==1:
                                print "no, here!"
                                result=(self.cur.fetchone(),)
                            else:
                                print "here!"
                                result=self.cur.fetchmany(max_rows)

            failures=0
            last_call_time=time()
        except self.Database_Error, mess:
            failures=failures+1
            if (failures > 1000 or time()-last_call_time > 600):
                # Hm. maybe the db is hosed.  Let's try again.
                failures=0
                last_call_time=time()
                return self.query(query_string, max_rows)
            else: raise sys.exc_type, sys.exc_value, sys.exc_traceback

        if desc is None:
            return (),()

        # Above, fetchmany returns None for empty result sets.  Maybe it
        # should return (), but it doesn't and Zope expects () so we'll fix
        # that here
        if result is None:
            result=()

        items=[]
        for name, type, length in desc:
            if type=='NUMBER':
                type='n'
            elif type=='DATE':
                type='d'
            else: type='s'
            items.append({
                'name': name,
                'type': type,})
        return items, result
