namespace VisualSPH_GUI
{
    partial class FrmVisualSPH
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.txtHeighFile = new System.Windows.Forms.TextBox();
            this.btnBrowse = new System.Windows.Forms.Button();
            this.txtFilePattern = new System.Windows.Forms.TextBox();
            this.grpView = new System.Windows.Forms.GroupBox();
            this.grpFrames = new System.Windows.Forms.GroupBox();
            this.lblPlayParameters = new System.Windows.Forms.Label();
            this.lblStartName = new System.Windows.Forms.Label();
            this.lblLastName = new System.Windows.Forms.Label();
            this.lblStep = new System.Windows.Forms.Label();
            this.txtStartName = new System.Windows.Forms.TextBox();
            this.txtLastName = new System.Windows.Forms.TextBox();
            this.txtStep = new System.Windows.Forms.TextBox();
            this.lblFilePattern = new System.Windows.Forms.Label();
            this.lblFolder = new System.Windows.Forms.Label();
            this.cmbView = new System.Windows.Forms.ComboBox();
            this.lblScreenResolution = new System.Windows.Forms.Label();
            this.lblGridResolution = new System.Windows.Forms.Label();
            this.cmbScreenResolution = new System.Windows.Forms.ComboBox();
            this.cmbGridResolution = new System.Windows.Forms.ComboBox();
            this.btnSave = new System.Windows.Forms.Button();
            this.btnStart = new System.Windows.Forms.Button();
            this.btnExit = new System.Windows.Forms.Button();
            this.grpView.SuspendLayout();
            this.grpFrames.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtHeighFile
            // 
            this.txtHeighFile.Location = new System.Drawing.Point(13, 26);
            this.txtHeighFile.Name = "txtHeighFile";
            this.txtHeighFile.Size = new System.Drawing.Size(149, 20);
            this.txtHeighFile.TabIndex = 0;
            // 
            // btnBrowse
            // 
            this.btnBrowse.Location = new System.Drawing.Point(168, 26);
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(75, 23);
            this.btnBrowse.TabIndex = 1;
            this.btnBrowse.Text = "Browse";
            this.btnBrowse.UseVisualStyleBackColor = true;
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // txtFilePattern
            // 
            this.txtFilePattern.Location = new System.Drawing.Point(13, 65);
            this.txtFilePattern.Name = "txtFilePattern";
            this.txtFilePattern.Size = new System.Drawing.Size(149, 20);
            this.txtFilePattern.TabIndex = 2;
            this.txtFilePattern.TextChanged += new System.EventHandler(this.txtFilePattern_TextChanged);
            // 
            // grpView
            // 
            this.grpView.Controls.Add(this.cmbView);
            this.grpView.Location = new System.Drawing.Point(271, 110);
            this.grpView.Name = "grpView";
            this.grpView.Size = new System.Drawing.Size(147, 71);
            this.grpView.TabIndex = 3;
            this.grpView.TabStop = false;
            this.grpView.Text = "View";
            this.grpView.Enter += new System.EventHandler(this.grpView_Enter);
            // 
            // grpFrames
            // 
            this.grpFrames.Controls.Add(this.txtStep);
            this.grpFrames.Controls.Add(this.txtLastName);
            this.grpFrames.Controls.Add(this.txtStartName);
            this.grpFrames.Controls.Add(this.lblStep);
            this.grpFrames.Controls.Add(this.lblLastName);
            this.grpFrames.Controls.Add(this.lblStartName);
            this.grpFrames.Location = new System.Drawing.Point(43, 110);
            this.grpFrames.Name = "grpFrames";
            this.grpFrames.Size = new System.Drawing.Size(212, 71);
            this.grpFrames.TabIndex = 4;
            this.grpFrames.TabStop = false;
            this.grpFrames.Text = "Frames";
            this.grpFrames.Enter += new System.EventHandler(this.grpFrames_Enter);
            // 
            // lblPlayParameters
            // 
            this.lblPlayParameters.AutoSize = true;
            this.lblPlayParameters.Location = new System.Drawing.Point(12, 94);
            this.lblPlayParameters.Name = "lblPlayParameters";
            this.lblPlayParameters.Size = new System.Drawing.Size(82, 13);
            this.lblPlayParameters.TabIndex = 5;
            this.lblPlayParameters.Text = "Play parameters";
            // 
            // lblStartName
            // 
            this.lblStartName.AutoSize = true;
            this.lblStartName.Location = new System.Drawing.Point(7, 20);
            this.lblStartName.Name = "lblStartName";
            this.lblStartName.Size = new System.Drawing.Size(58, 13);
            this.lblStartName.TabIndex = 0;
            this.lblStartName.Text = "Start name";
            this.lblStartName.Click += new System.EventHandler(this.lblStartName_Click);
            // 
            // lblLastName
            // 
            this.lblLastName.AutoSize = true;
            this.lblLastName.Location = new System.Drawing.Point(80, 20);
            this.lblLastName.Name = "lblLastName";
            this.lblLastName.Size = new System.Drawing.Size(56, 13);
            this.lblLastName.TabIndex = 1;
            this.lblLastName.Text = "Last name";
            this.lblLastName.Click += new System.EventHandler(this.lblLastName_Click);
            // 
            // lblStep
            // 
            this.lblStep.AutoSize = true;
            this.lblStep.Location = new System.Drawing.Point(154, 20);
            this.lblStep.Name = "lblStep";
            this.lblStep.Size = new System.Drawing.Size(29, 13);
            this.lblStep.TabIndex = 2;
            this.lblStep.Text = "Step";
            this.lblStep.Click += new System.EventHandler(this.lblStep_Click);
            // 
            // txtStartName
            // 
            this.txtStartName.Location = new System.Drawing.Point(10, 37);
            this.txtStartName.Name = "txtStartName";
            this.txtStartName.Size = new System.Drawing.Size(55, 20);
            this.txtStartName.TabIndex = 3;
            this.txtStartName.TextChanged += new System.EventHandler(this.txtStartName_TextChanged);
            // 
            // txtLastName
            // 
            this.txtLastName.Location = new System.Drawing.Point(83, 37);
            this.txtLastName.Name = "txtLastName";
            this.txtLastName.Size = new System.Drawing.Size(53, 20);
            this.txtLastName.TabIndex = 4;
            this.txtLastName.TextChanged += new System.EventHandler(this.txtLastName_TextChanged);
            // 
            // txtStep
            // 
            this.txtStep.Location = new System.Drawing.Point(157, 37);
            this.txtStep.Name = "txtStep";
            this.txtStep.Size = new System.Drawing.Size(49, 20);
            this.txtStep.TabIndex = 5;
            this.txtStep.TextChanged += new System.EventHandler(this.txtStep_TextChanged);
            // 
            // lblFilePattern
            // 
            this.lblFilePattern.AutoSize = true;
            this.lblFilePattern.Location = new System.Drawing.Point(12, 49);
            this.lblFilePattern.Name = "lblFilePattern";
            this.lblFilePattern.Size = new System.Drawing.Size(59, 13);
            this.lblFilePattern.TabIndex = 6;
            this.lblFilePattern.Text = "File pattern";
            this.lblFilePattern.Click += new System.EventHandler(this.lblFilePattern_Click);
            // 
            // lblFolder
            // 
            this.lblFolder.AutoSize = true;
            this.lblFolder.Location = new System.Drawing.Point(13, 13);
            this.lblFolder.Name = "lblFolder";
            this.lblFolder.Size = new System.Drawing.Size(36, 13);
            this.lblFolder.TabIndex = 7;
            this.lblFolder.Text = "Folder";
            this.lblFolder.Click += new System.EventHandler(this.lblFolder_Click);
            // 
            // cmbView
            // 
            this.cmbView.FormattingEnabled = true;
            this.cmbView.Location = new System.Drawing.Point(7, 37);
            this.cmbView.Name = "cmbView";
            this.cmbView.Size = new System.Drawing.Size(121, 21);
            this.cmbView.TabIndex = 0;
            // 
            // lblScreenResolution
            // 
            this.lblScreenResolution.AutoSize = true;
            this.lblScreenResolution.Location = new System.Drawing.Point(13, 197);
            this.lblScreenResolution.Name = "lblScreenResolution";
            this.lblScreenResolution.Size = new System.Drawing.Size(89, 13);
            this.lblScreenResolution.TabIndex = 8;
            this.lblScreenResolution.Text = "Screen resolution";
            // 
            // lblGridResolution
            // 
            this.lblGridResolution.AutoSize = true;
            this.lblGridResolution.Location = new System.Drawing.Point(253, 197);
            this.lblGridResolution.Name = "lblGridResolution";
            this.lblGridResolution.Size = new System.Drawing.Size(74, 13);
            this.lblGridResolution.TabIndex = 9;
            this.lblGridResolution.Text = "Grid resolution";
            // 
            // cmbScreenResolution
            // 
            this.cmbScreenResolution.FormattingEnabled = true;
            this.cmbScreenResolution.Location = new System.Drawing.Point(16, 213);
            this.cmbScreenResolution.Name = "cmbScreenResolution";
            this.cmbScreenResolution.Size = new System.Drawing.Size(146, 21);
            this.cmbScreenResolution.TabIndex = 10;
            // 
            // cmbGridResolution
            // 
            this.cmbGridResolution.FormattingEnabled = true;
            this.cmbGridResolution.Location = new System.Drawing.Point(256, 214);
            this.cmbGridResolution.Name = "cmbGridResolution";
            this.cmbGridResolution.Size = new System.Drawing.Size(143, 21);
            this.cmbGridResolution.TabIndex = 11;
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(151, 254);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(75, 38);
            this.btnSave.TabIndex = 12;
            this.btnSave.Text = "Save";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(243, 254);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(75, 38);
            this.btnStart.TabIndex = 13;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // btnExit
            // 
            this.btnExit.Location = new System.Drawing.Point(334, 254);
            this.btnExit.Name = "btnExit";
            this.btnExit.Size = new System.Drawing.Size(84, 38);
            this.btnExit.TabIndex = 14;
            this.btnExit.Text = "Exit";
            this.btnExit.UseVisualStyleBackColor = true;
            this.btnExit.Click += new System.EventHandler(this.btnExit_Click);
            // 
            // FrmVisualSPH
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(441, 304);
            this.Controls.Add(this.btnExit);
            this.Controls.Add(this.btnStart);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.cmbGridResolution);
            this.Controls.Add(this.cmbScreenResolution);
            this.Controls.Add(this.lblGridResolution);
            this.Controls.Add(this.lblScreenResolution);
            this.Controls.Add(this.lblFolder);
            this.Controls.Add(this.lblFilePattern);
            this.Controls.Add(this.lblPlayParameters);
            this.Controls.Add(this.grpFrames);
            this.Controls.Add(this.grpView);
            this.Controls.Add(this.txtFilePattern);
            this.Controls.Add(this.btnBrowse);
            this.Controls.Add(this.txtHeighFile);
            this.Name = "FrmVisualSPH";
            this.Text = "VisualSPH";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.grpView.ResumeLayout(false);
            this.grpFrames.ResumeLayout(false);
            this.grpFrames.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtHeighFile;
        private System.Windows.Forms.Button btnBrowse;
        private System.Windows.Forms.TextBox txtFilePattern;
        private System.Windows.Forms.GroupBox grpView;
        private System.Windows.Forms.GroupBox grpFrames;
        private System.Windows.Forms.TextBox txtStep;
        private System.Windows.Forms.TextBox txtLastName;
        private System.Windows.Forms.TextBox txtStartName;
        private System.Windows.Forms.Label lblStep;
        private System.Windows.Forms.Label lblLastName;
        private System.Windows.Forms.Label lblStartName;
        private System.Windows.Forms.Label lblPlayParameters;
        private System.Windows.Forms.Label lblFilePattern;
        private System.Windows.Forms.Label lblFolder;
        private System.Windows.Forms.ComboBox cmbView;
        private System.Windows.Forms.Label lblScreenResolution;
        private System.Windows.Forms.Label lblGridResolution;
        private System.Windows.Forms.ComboBox cmbScreenResolution;
        private System.Windows.Forms.ComboBox cmbGridResolution;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.Button btnExit;
    }
}

