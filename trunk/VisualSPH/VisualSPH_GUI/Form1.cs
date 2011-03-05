using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace VisualSPH_GUI
{
    public partial class FrmVisualSPH : Form
    {
        public FrmVisualSPH()
        {
            InitializeComponent();
        }

        private void btnBrowse_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog folderBrowserDialog1 = new FolderBrowserDialog();
            string folderName = "";
            DialogResult result = folderBrowserDialog1.ShowDialog();
            if (result == DialogResult.OK)
            {
               folderName = folderBrowserDialog1.SelectedPath;
            }
            txtHeighFile.Text = folderName;
        }

        private void PushToConfigFile()
        {
            FileStream filestream = new FileStream(Application.StartupPath + "\\settings.txt", FileMode.OpenOrCreate, FileAccess.Write);

            filestream.SetLength(0);

            StreamWriter streamwriter = new StreamWriter(filestream);

            streamwriter.Write(txtHeighFile.Text + "\\");
            streamwriter.Write("\n");
            streamwriter.Write(txtFilePattern.Text);
            streamwriter.Write("\n");
            streamwriter.Write(txtStartName.Text);
            streamwriter.Write("\n");
            streamwriter.Write(txtLastName.Text);
            streamwriter.Write("\n");
            streamwriter.Write(txtStep.Text);
            streamwriter.Write("\n" + "5 5 1" + "\n" + "0 0 0");
            streamwriter.Write("\n");
            streamwriter.Write(cmbScreenResolution.Text);
            streamwriter.Write("\n");
            streamwriter.Write(cmbGridResolution.Text);
            streamwriter.Write("\n");
            streamwriter.Write(cmbView.Text);

            streamwriter.Flush();
            streamwriter.Close();
            filestream.Close();
        }
        private void txtFilePattern_TextChanged(object sender, EventArgs e)
        {

        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void txtStartName_TextChanged(object sender, EventArgs e)
        {

        }

        private void txtLastName_TextChanged(object sender, EventArgs e)
        {

        }

        private void txtStep_TextChanged(object sender, EventArgs e)
        {

        }

        private void grpFrames_Enter(object sender, EventArgs e)
        {

        }

        private void lblStartName_Click(object sender, EventArgs e)
        {

        }

        private void lblLastName_Click(object sender, EventArgs e)
        {

        }

        private void lblStep_Click(object sender, EventArgs e)
        {

        }

        private void grpView_Enter(object sender, EventArgs e)
        {

        }

        private void lblFolder_Click(object sender, EventArgs e)
        {

        }

        private void lblFilePattern_Click(object sender, EventArgs e)
        {

        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            if (txtHeighFile.Text != String.Empty)
            {
                PushToConfigFile();
                System.Diagnostics.Process.Start(Application.StartupPath + "\\Render.exe");
            }
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            PushToConfigFile();
        }

        private void btnExit_Click(object sender, EventArgs e)
        {
            Application.Exit();   // exit button
        }

        private void menuStrip1_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {

        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();  // exit button
        }

        private void cmbGridResolution_SelectedIndexChanged(object sender, EventArgs e)
        {

        }
    }
}
