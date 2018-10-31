using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.Concurrent;

namespace HPSocketCS
{
    public class Extra<T>
    {
        ConcurrentDictionary<IntPtr, T> dict = new ConcurrentDictionary<IntPtr, T>();

        /// <summary>
        /// 获取附加数据
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public T Get(IntPtr key)
        {
            T value;
            if (dict.TryGetValue(key, out value))
            {
                return value;
            }
            return default(T);
        }

        /// <summary>
        /// 设置附加数据
        /// 
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        public bool Set(IntPtr key, T newValue)
        {
            try
            {
                dict.AddOrUpdate(key, newValue, (tKey, existingVal) => { return newValue; });
                return true;
            }
            catch (OverflowException)
            {
                // 字典数目超过int.max
                return false;
            }
            catch (ArgumentNullException)
            {
                // 参数为空
                return false;
            }
            catch (Exception)
            {
                return false;
            }
        }

        /// <summary>
        /// 删除附加数据
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public bool Remove(IntPtr key)
        {
            T value;
            return dict.TryRemove(key, out value);
        }
    }


    public class ConnectionExtra
    {
        ConcurrentDictionary<IntPtr, object> dict = new ConcurrentDictionary<IntPtr, object>();

        /// <summary>
        /// 获取附加数据
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public object GetExtra(IntPtr key)
        {
            object value;
            if (dict.TryGetValue(key, out value))
            {
                return value;
            }
            return null;
        }

        /// <summary>
        /// 获取附加数据
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public T GetExtra<T>(IntPtr key)
        {
            object value;
            if (dict.TryGetValue(key, out value))
            {
                return (T)value;
            }
            return default(T);
        }

        /// <summary>
        /// 设置附加数据
        /// 
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        public bool SetExtra(IntPtr key, object newValue)
        {
            try
            {
                dict.AddOrUpdate(key, newValue, (tKey, existingVal) => { return newValue; });
                return true;
            }
            catch (OverflowException)
            {
                // 字典数目超过int.max
                return false;
            }
            catch (ArgumentNullException)
            {
                // 参数为空
                return false;
            }
            catch (Exception)
            {
                return false;
            }
        }

        /// <summary>
        /// 删除附加数据
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public bool RemoveExtra(IntPtr key)
        {
            object value;
            return dict.TryRemove(key, out value);
        }
    }
}
