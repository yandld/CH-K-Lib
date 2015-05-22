using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace serial
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            System.Threading.Mutex mutex = new System.Threading.Mutex(false, "ThisShouldOnlyRunOnce");
            bool Running = !mutex.WaitOne(0, false);
            if (!Running)
                Application.Run(new Main());
            else
                MessageBox.Show("程序已启动！", "错误");
        }
    }
}
