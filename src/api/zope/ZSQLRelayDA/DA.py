database_type='SQLRelay'
__doc__='''%s Database Connection

$Id: DA.py,v 1.1.1.1 2002-11-16 06:49:52 mused Exp $''' % database_type
__version__='$Revision: 1.1.1.1 $'[11:-2]

from db import DB
import Shared.DC.ZRDB.Connection, sys, DABase
from Globals import HTMLFile
from ImageFile import ImageFile
from ExtensionClass import Base

manage_addZSQLRelayConnectionForm=HTMLFile('connectionAdd',globals())

def manage_addZSQLRelayConnection(self, id, title,
                                connection_string,
                                check=None, REQUEST=None):
    """Add a DB connection to a folder"""
    self._setObject(id, Connection(
	id, title, connection_string, check))
    if REQUEST is not None: return self.manage_main(self,REQUEST)

class Connection(DABase.Connection):
    " "
    database_type=database_type
    id='%s_database_connection' % database_type
    meta_type=title='Z %s Database Connection' % database_type
    icon='misc_/Z%sDA/conn' % database_type

    def factory(self):
        return DB

    def table_info(self):
        return self._v_database_connection.table_info()

classes=('DA.Connection',)

meta_types=(
    {'name':'Z %s Database Connection' % database_type,
     'action':'manage_addZ%sConnectionForm' % database_type,
     },
    )

folder_methods={
    'manage_addZSQLRelayConnection':
    manage_addZSQLRelayConnection,
    'manage_addZSQLRelayConnectionForm':
    manage_addZSQLRelayConnectionForm,
    }

__ac_permissions__=(
    ('Add Z SQLRelay Database Connections',
     ('manage_addZSQLRelayConnectionForm',
      'manage_addZSQLRelayConnection')),
    )

misc_={
    'conn':   ImageFile('Shared/DC/ZRDB/www/DBAdapterFolder_icon.gif'),
    }

for icon in ('table', 'view', 'stable', 'what',
             'field', 'text','bin','int','float',
             'date','time','datetime'):
    misc_[icon]=ImageFile('icons/%s.gif' % icon, globals())
    
