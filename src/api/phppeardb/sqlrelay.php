<?php
/* vim: set expandtab tabstop=4 shiftwidth=4: */
// +----------------------------------------------------------------------+
// | PHP Version 4                                                        |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997-2003 The PHP Group                                |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.02 of the PHP license,      |
// | that is bundled with this package in the file COPYING, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Author: David Muse <ssb@php.net>                                    |
// +----------------------------------------------------------------------+
//
// $Id: sqlrelay.php,v 1.8 2005-09-01 03:14:39 mused Exp $
//
// Database independent query interface definition for PHP's SQLRelay
// extension.
//

require_once "DB/common.php";

class DB_sqlrelay_cursor
{
    var $cursor;
    var $identity = "";
    var $connection;
    var $rownum = 0;

    function DB_sqlrelay_cursor($cur,$connection)
    {
        $this->cursor = $cur;
        $this->connection = $connection;
        $this->rownun = 0;
    }
}

class DB_sqlrelay extends DB_common
{
    // {{{ properties

    var $connection;
    var $phptype, $dbsyntax;
    var $autocommit = false;
    var $fetchmode = DB_FETCHMODE_ORDERED; /* Default fetch mode */
    var $affectedrows = 0;

    // }}}
    // {{{ constructor

    /**
     * DB_sqlrelay constructor.
     *
     * @access public
     */

    function DB_sqlrelay()
    {
        $this->DB_common();
        $this->phptype = 'sqlrelay';
        $this->dbsyntax = 'sqlrelay';
        $this->features = array(
            'prepare' => true,
            'pconnect' => true,
            'transactions' => true,
            'limit' => 'emulate'
        );
    }

    // }}}
    // {{{ connect()

    /**
     * Connect to a database and log in as the specified user.
     *
     * @param $dsn the data source name (see DB::parseDSN for syntax)
     * @param $persistent (optional) whether the connection should
     *        be persistent
     * @access public
     * @return int DB_OK on success, a DB error on failure
     */

    function connect($dsninfo, $persistent = false)
    {


        if (method_exists($this,'loadExtension') || 
                            method_exists($this,'loadextension')) {
            if (!PEAR::loadExtension('sql_relay')) {
                return $this->raiseError(DB_ERROR_EXTENSION_NOT_FOUND);
            }
        } else {
            if (!DB::assertExtension('sql_relay')) {
                return $this->raiseError(DB_ERROR_EXTENSION_NOT_FOUND);
            }
        }

        $this->dsn = $dsninfo;

        $this->connection = sqlrcon_alloc($dsninfo['hostspec'],
                                            $dsninfo['port'],
                                            $dsninfo['socket'],
                                            $dsninfo['username'],
                                            $dsninfo['password'],
                                            0,1);
        return DB_OK;
    }

    // }}}
    // {{{ disconnect()

    /**
     * Log out and disconnect from the database.
     *
     * @access public
     *
     * @return bool TRUE on success, FALSE if not connected.
     */
    function disconnect()
    {
        sqlrcon_free($this->connection);
        $this->connection = null;
        return true;
    }

    // }}}
    // {{{ simpleQuery()

    /**
     * Send a query to SQLRelay and return the results as a SQLRelay cursor.
     *
     * @param the SQL query
     *
     * @access public
     *
     * @return mixed returns a valid SQLRelay cursor for successful SELECT
     * queries, DB_OK for other successful queries.  A DB error is
     * returned on failure.
     */
    function simpleQuery($query)
    {

        $cursor = sqlrcur_alloc($this->connection);
        sqlrcur_setResultSetBufferSize($cursor,100);
        if (!sqlrcur_sendQuery($cursor, $query)) {
            $this->affectedrows = 0;
            $error = sqlrcur_errorMessage($cursor);
            sqlrcur_free($cursor);
            return $this->raiseError(DB_ERROR, null, null, null, $error);
        }

        $this->affectedrows = sqlrcur_affectedRows($cursor);

        /* If the query was a select, return a cursor, otherwise return DB_OK.
        If there are any affected rows, then the query was definitely not a
        select, otherwise there's no good way to know what kind of query it was
        except by parsing it. */
        if ($this->affectedrows <= 0 &&
                preg_match('/^\s*\(?\s*SELECT\s+/si', $query) &&
                    !preg_match('/^\s*\(?\s*SELECT\s+INTO\s/si', $query)) {
            return new DB_sqlrelay_cursor($cursor);
        }

        sqlrcur_free($cursor);
        return DB_OK;
    }

    // }}}
    // {{{ prepare()

    /**
    * Prepares a query for multiple execution with execute().
    *
    * @param string the query to prepare
    *
    * @return SQLRelay cursor
    *
    * @access public
    * @see execute
    */

    function prepare($query)
    {
        $cursor = sqlrcur_alloc($this->connection);
        sqlrcur_setResultSetBufferSize($cursor,100);
        sqlrcur_prepareQuery($cursor, $query);
        return new DB_sqlrelay_cursor($cursor,$connection);
    }

    // }}}
    // {{{ autoExecute()

    /**
     * Automaticaly generates an insert or update query and call prepare()
     * and execute() with it
     *
     * @param string $table         the table name
     * @param array  $fields_values the associative array where $key is a
     *                               field name and $value its value
     * @param int    $mode          a type of query to make:
     *                               DB_AUTOQUERY_INSERT or DB_AUTOQUERY_UPDATE
     * @param string $where         for update queries: the WHERE clause to
     *                               append to the SQL statement.  Don't
     *                               include the "WHERE" keyword.
     *
     * @return mixed  a new DB_result object for successful SELECT queries
     *                 or DB_OK for successul data manipulation queries.
     *                 A DB_Error object on failure.
     *
     * @uses DB_common::autoPrepare(), DB_common::execute()
     */
    function autoExecute($table, $fields_values, $mode = DB_AUTOQUERY_INSERT,
                         $where = false)
    {
        $sth = $this->autoPrepare($table, array_keys($fields_values), $mode,
                                  $where);
        if (DB::isError($sth)) {
            return $sth;
        }
        $ret =& $this->execute($sth, $fields_values);
        $this->freePrepared($sth);
        return $ret;

    }

    // }}}
    // {{{ buildManipSQL()

    /**
     * Produces an SQL query string for autoPrepare()
     *
     * Example:
     * <pre>
     * buildManipSQL('table_sql', array('field1', 'field2', 'field3'),
     *               DB_AUTOQUERY_INSERT);
     * </pre>
     *
     * That returns
     * <samp>
     * INSERT INTO table_sql (field1,field2,field3) VALUES (?,?,?)
     * </samp>
     *
     * NOTES:
     *   - This belongs more to a SQL Builder class, but this is a simple
     *     facility.
     *   - Be carefull! If you don't give a $where param with an UPDATE
     *     query, all the records of the table will be updated!
     *
     * @param string $table         the table name
     * @param array  $table_fields  the array of field names
     * @param int    $mode          a type of query to make:
     *                               DB_AUTOQUERY_INSERT or DB_AUTOQUERY_UPDATE
     * @param string $where         for update queries: the WHERE clause to
     *                               append to the SQL statement.  Don't
     *                               include the "WHERE" keyword.
     *
     * @return string  the sql query for autoPrepare()
     */
    function buildManipSQL($table, $table_fields, $mode, $where = false)
    {
        if ($identity == "") {
            $idenity = sqlrcon_identify($this->connection);
        }
        if (count($table_fields) == 0) {
            return $this->raiseError(DB_ERROR_NEED_MORE_DATA);
        }
        $first = true;
        switch ($mode) {
            case DB_AUTOQUERY_INSERT:
                $values = '';
                $names = '';
                foreach ($table_fields as $value) {
                    if ($first) {
                        $first = false;
                    } else {
                        $names .= ',';
                        $values .= ',';
                    }
                    $names .= $value;
                    if ($identity != "sybase" && $identity != "freetds") {
                        $values .= ":$value";
                    } else {
                        $values .= "@$value";
                    }
                }
                return "INSERT INTO $table ($names) VALUES ($values)";
            case DB_AUTOQUERY_UPDATE:
                $set = '';
                foreach ($table_fields as $value) {
                    if ($first) {
                        $first = false;
                    } else {
                        $set .= ',';
                    }
                    if ($identity != "sybase" && $identity != "freetds") {
                        $set .= "$value = :$value";
                    } else {
                        $set .= "$value = @$value";
                    }
                }
                $sql = "UPDATE $table SET $set";
                if ($where) {
                    $sql .= " WHERE $where";
                }
                return $sql;
            default:
                return $this->raiseError(DB_ERROR_SYNTAX);
        }
    }

    // }}}
    // {{{ execute()
    /**
    * Executes a prepared SQL query
    * With execute() the generic query of prepare is
    * assigned with the given data array. The values
    * of the array inserted into the query in the same
    * order like the array order
    *
    * @param resource $cursor query handle from prepare()
    * @param array    $data numeric array containing the
    *                       data to insert into the query
    *
    * @return mixed  a new DB_Result or a DB_Error when fail
    *
    * @access public
    * @see prepare()
    */

    function &execute(&$sqlrcursor, $data = false)
    {
        sqlrcur_clearBinds($sqlrcursor->cursor);
        if ($data) {
            foreach ($data as $index=>$value) {
                sqlrcur_inputBind($sqlrcursor->cursor, $index, $value);
            }
        }
        if (!sqlrcur_executeQuery($sqlrcursor->cursor)) {
            $error = sqlrcur_errorMessage($sqlrcursor->cursor);
            $this->freeResult($sqlrcursor);
            return $this->raiseError(DB_ERROR, null, null, null, $error);
        }
        return $sqlrcursor;
    }

    // }}}
    // {{{ fetchInto()

    /**
     * Fetch a row and insert the data into an existing array.
     *
     * @param $cursor SQLRelay cursor
     * @param $arr (reference) array where data from the row is stored
     * @param $fetchmode how the array data should be indexed
     * @param   $rownum the row number to fetch
     * @access public
     *
     * @return int DB_OK on success, a DB error on failure
     */
    function fetchInto(&$sqlrcursor, &$arr, $fetchmode, $rownum = null)
    {
        if ($rownum != null) {
            $sqlrcursor->rownum = $rownum;
        }
        if ($fetchmode & DB_FETCHMODE_ASSOC) {
            $arr = sqlrcur_getRowAssoc($sqlrcursor->cursor,
                                        $sqlrcursor->rownum);
        } else {
            $arr = sqlrcur_getRow($sqlrcursor->cursor,
                                        $sqlrcursor->rownum);
        }
        if (!$arr) {
            // See: http://bugs.php.net/bug.php?id=22328
            // for why we can't check errors on fetching
            return null;
        }
        $sqlrcursor->rownum++;
        return DB_OK;
    }

    // }}}
    // {{{ freeResult()

    /**
     * Free the internal resources associated with $cursor.
     *
     * @param $cursor SQLRelay cursor
     *
     * @access public
     *
     * @return bool TRUE
     */
    function freeResult(&$sqlrcursor)
    {
        if (is_resource($sqlrcursor)) {
            sqlrcur_free($sqlrcursor->cursor);
        }
        return true;
    }

    // }}}
    // {{{ freePrepared()

    /**
     * Frees the internal resources associated with a prepared query
     *
     * @param resource $stmt           the prepared statement's PHP resource
     * @param bool     $free_resource  should the PHP resource be freed too?
     *                                  Use false if you need to get data
     *                                  from the result set later.
     *
     * @return bool  TRUE on success, FALSE if $result is invalid
     *
     * @see DB_common::prepare()
     */
    function freePrepared($stmt, $free_resource = true)
    {
        return true;
    }
    // {{{ numCols()

    /**
     * Get the number of columns in a cursor set.
     *
     * @param $cursor SQLRelay cursor
     *
     * @access public
     *
     * @return int the number of columns per row in $cursor
     */
    function numCols($sqlrcursor)
    {
        return sqlrcur_colCount($sqlrcursor->cursor);
    }

    // }}}
    // {{{ numRows()

    /**
     * Get the number of rows in a result set.
     *
     * @param $cursor SQLRelay cursor
     *
     * @access public
     *
     * @return int the number of rows in $cursor
     */
    function numRows($sqlrcursor)
    {
        return sqlrcur_rowCount($sqlrcursor->cursor);
    }

    // }}}
    // {{{ autoCommit()

    /**
     * Enable/disable automatic commits
     */
    function autoCommit($onoff = false)
    {
        if ($onoff == true) {
            sqlrcon_autoCommitOn($this->connection);
        } else {
            sqlrcon_autoCommitOff($this->connection);
        }
        return DB_OK;
    }

    // }}}
    // {{{ commit()

    /**
     * Commit the current transaction.
     */
    function commit()
    {
        if (sqlrcon_commit($this->connection) == 1) {
            return DB_OK;
        } else {
            return $this->raiseError(DB_ERROR, null, null, null,
                                    sqlrcon_errorMessage($this->connection));
        }
    }

    // }}}
    // {{{ rollback()

    /**
     * Roll back (undo) the current transaction.
     */
    function rollback()
    {
        if (sqlrcon_rollback($this->connection) == 1) {
            return DB_OK;
        } else {
            return $this->raiseError(DB_ERROR, null, null, null,
                                    sqlrcon_errorMessage($this->connection));
        }
    }

    // }}}
    // {{{ affectedRows()

    /**
     * Gets the number of rows affected by the data manipulation
     * query.  For other queries, this function returns 0.
     *
     * @return number of rows affected by the last query
     */

    function affectedRows()
    {
        return $this->affectedrows;
    }

    // }}}
    // {{{ tableInfo()

    function tableInfo($sqlrcursor, $mode = null)
    {

        /*
         * depending on $mode, metadata returns the following values:
         *
         * - mode is null (default):
         * $result[]:
         *   [0]["table"]  table name
         *   [0]["name"]   field name
         *   [0]["type"]   field type
         *   [0]["len"]    field length
         *   [0]["flags"]  field flags
         *
         * - mode is DB_TABLEINFO_ORDER
         * $result[]:
         *   ["num_fields"] number of metadata records
         *   [0]["table"]  table name
         *   [0]["name"]   field name
         *   [0]["type"]   field type
         *   [0]["len"]    field length
         *   [0]["flags"]  field flags
         *   ["order"][field name]  index of field named "field name"
         *   The last one is used, if you have a field name, but no index.
         *   Test:  if (isset($result['meta']['myfield'])) { ...
         *
         * - mode is DB_TABLEINFO_ORDERTABLE
         *    the same as above. but additionally
         *   ["ordertable"][table name][field name] index of field
         *      named "field name"
         *
         *      this is, because if you have fields from different
         *      tables with the same field name * they override each
         *      other with DB_TABLEINFO_ORDER
         *
         *      you can combine DB_TABLEINFO_ORDER and
         *      DB_TABLEINFO_ORDERTABLE with DB_TABLEINFO_ORDER |
         *      DB_TABLEINFO_ORDERTABLE * or with DB_TABLEINFO_FULL
         */

        if (is_string($sqlrcursor)) {
            // if $result is a string, then we want information about a
            // table without a resultset else we want information about a
            // resultset, this is not yet supported
            return null;
        } else if (empty($sqlrcursor)) {
            return null;
        } else if ($sqlrcursor==DB_OK) {
            return null;
        }

        $count = sqlrcur_colCount($sqlrcursor->cursor);

        if (empty($mode)) {
            for ($i=0; $i<$count; $i++) {
                $res[$i]['table'] = '';
                $res[$i]['name'] = sqlrcur_getColumnName($sqlrcursor->cursor,$i);
                $res[$i]['type'] = sqlrcur_getColumnType($sqlrcursor->cursor,$i);
                $res[$i]['len'] = sqlrcur_getColumnLength($sqlrcursor->cursor,$i);
                $res[$i]['flags'] = '';
            }
        } else {
            $res['num_fields']= $count;

            for ($i=0; $i<$count; $i++) {
                $res[$i]['table'] = '';
                $res[$i]['name'] = sqlrcur_getColumnName($sqlrcursor->cursor,$i);
                $res[$i]['type'] = sqlrcur_getColumnType($sqlrcursor->cursor,$i);
                $res[$i]['len'] = sqlrcur_getColumnLength($sqlrcursor->cursor,$i);
                $res[$i]['flags'] = '';
                if ($mode & DB_TABLEINFO_ORDER) {
                    $res['order'][$res[$i]['name']] = $i;
                }
                if ($mode & DB_TABLEINFO_ORDERTABLE) {
                    $res['ordertable'][$res[$i]['table']][$res[$i]['name']] = $i;
                }
            }
        }

        return $res;

    }

    // }}}
    // {{{ errorNative()

    /**
     * Get the native error code of the last error (if any) that
     * occured on the current connection.
     *
     * @access public
     *
     * @return int native SQLRelay error code
     */

    function errorNative()
    {
        # FIXME: sqlrelay doesn't have error codes, what do I do?
        #return sqlrcon_errorMessage($this->connection);
        return DB_ERROR;
    }

    // }}}
}

?>
