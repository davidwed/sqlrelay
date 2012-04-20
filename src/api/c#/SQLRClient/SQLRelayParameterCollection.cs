using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Data;
using System.Globalization;

namespace SQLRClient
{
    class SQLRelayParameterCollection : ArrayList, IDataParameterCollection
    {
        public object this[string index]
        {
            get
            {
                return this[IndexOf(index)];
            }
            set
            {
                this[IndexOf(index)] = value;
            }
        }

        public bool Contains(string parametername)
        {
            return (-1 != IndexOf(parametername));
        }

        public int IndexOf(string parametername)
        {
            int index = 0;
            foreach (SQLRelayParameter item in this)
            {
                if (0 == cultureAwareCompare(item.ParameterName, parametername))
                {
                    return index;
                }
                index++;
            }
            return -1;
        }

        public void RemoveAt(string parametername)
        {
            RemoveAt(IndexOf(parametername));
        }

        public override int Add(object value)
        {
            return Add((SQLRelayParameter)value);
        }

        public int Add(SQLRelayParameter value)
        {
            if (((SQLRelayParameter)value).ParameterName != null)
            {
                return base.Add(value);
            }
            else
                throw new ArgumentException("parameter must be named");
        }

        public int Add(string parameterName, DbType type)
        {
            return Add(new SQLRelayParameter(parameterName, type));
        }

        public int Add(string parameterName, object value)
        {
            return Add(new SQLRelayParameter(parameterName, value));
        }

        public int Add(string parameterName, DbType dbType, string sourceColumn)
        {
            return Add(new SQLRelayParameter(parameterName, dbType, sourceColumn));
        }

        private int cultureAwareCompare(string stringa, string stringb)
        {
            return CultureInfo.CurrentCulture.CompareInfo.Compare(stringa, stringb, CompareOptions.IgnoreKanaType | CompareOptions.IgnoreWidth | CompareOptions.IgnoreCase);
        }
    }
}
