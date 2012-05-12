using System;
using System.Collections;
using System.Data;
using System.Globalization;

namespace SQLRClient
{
    public class SQLRelayParameterCollection : ArrayList, IDataParameterCollection
    {
        public Object this[String index]
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

        public Boolean  Contains(String parametername)
        {
            return (-1 != IndexOf(parametername));
        }

        public Int32 IndexOf(String parametername)
        {
            Int32 index = 0;
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

        public void RemoveAt(String parametername)
        {
            RemoveAt(IndexOf(parametername));
        }

        public override Int32 Add(Object value)
        {
            return Add((SQLRelayParameter)value);
        }

        public Int32 Add(SQLRelayParameter value)
        {
            if (((SQLRelayParameter)value).ParameterName != null)
            {
                return base.Add(value);
            }
            else
            {
                throw new ArgumentException("parameter must be named");
            }
        }

        public Int32 Add(String parameterName, DbType type)
        {
            return Add(new SQLRelayParameter(parameterName, type));
        }

        public Int32 Add(String parameterName, object value)
        {
            return Add(new SQLRelayParameter(parameterName, value));
        }

        public Int32 Add(String parameterName, DbType dbtype, String sourcecolumn)
        {
            return Add(new SQLRelayParameter(parameterName, dbtype, sourcecolumn));
        }

        private Int32 cultureAwareCompare(String stringa, String stringb)
        {
            return CultureInfo.CurrentCulture.CompareInfo.Compare(stringa, stringb, CompareOptions.IgnoreKanaType | CompareOptions.IgnoreWidth | CompareOptions.IgnoreCase);
        }
    }
}
