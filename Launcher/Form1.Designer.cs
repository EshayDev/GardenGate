namespace Launcher
{
    partial class Form1
    {
        private Label UsernameLabel;
        private TextBox UsernameTextBox;
        private Label ServerIpLabel;
        private TextBox ServerIpTextBox;
        private Label ServerPasswordLabel;
        private TextBox ServerPasswordTextBox;
        private Label GamePathLabel;
        private TextBox GamePathTextBox;
        private Button BrowseGameButton;
        private Label ModDataLabel;
        private ComboBox ModDataComboBox;
        private CheckBox ModDataCheckBox;
        private Label ArgsLabel;
        private TextBox ArgsTextBox;
        private Button LaunchButton;
        private RadioButton GW1Radio;
        private RadioButton GW2Radio;

        private void InitializeComponent()
        {
            GamePathLabel = new Label();
            GamePathTextBox = new TextBox();
            BrowseGameButton = new Button();
            UsernameLabel = new Label();
            UsernameTextBox = new TextBox();
            ServerIpLabel = new Label();
            ServerIpTextBox = new TextBox();
            ServerPasswordLabel = new Label();
            ServerPasswordTextBox = new TextBox();
            ModDataLabel = new Label();
            ModDataComboBox = new ComboBox();
            ModDataCheckBox = new CheckBox();
            ArgsLabel = new Label();
            ArgsTextBox = new TextBox();
            LaunchButton = new Button();
            GW1Radio = new RadioButton();
            GW2Radio = new RadioButton();
            SuspendLayout();

            UsernameLabel.Location = new Point(12, 30);
            UsernameLabel.Name = "UsernameLabel";
            UsernameLabel.Size = new Size(100, 23);
            UsernameLabel.Text = "Username:";
            UsernameTextBox.Location = new Point(120, 27);
            UsernameTextBox.Name = "UsernameTextBox";
            UsernameTextBox.Size = new Size(260, 23);
            UsernameTextBox.TabIndex = 1;
            ServerIpLabel.Location = new Point(12, 65);
            ServerIpLabel.Name = "ServerIpLabel";
            ServerIpLabel.Size = new Size(100, 23);
            ServerIpLabel.Text = "Server IP:";
            ServerIpTextBox.Location = new Point(120, 62);
            ServerIpTextBox.Name = "ServerIpTextBox";
            ServerIpTextBox.Size = new Size(260, 23);
            ServerIpTextBox.TabIndex = 2;
            ServerPasswordLabel.Location = new Point(12, 100);
            ServerPasswordLabel.Text = "Server Password:";
            ServerPasswordTextBox.Location = new Point(120, 97);
            ServerPasswordTextBox.Name = "ServerIpTextBox";
            ServerPasswordTextBox.Size = new Size(260, 23);
            ServerPasswordTextBox.TabIndex = 3;
            GamePathLabel.Location = new Point(12, 135);
            GamePathLabel.Name = "GamePathLabel";
            GamePathLabel.Size = new Size(100, 23);
            GamePathLabel.TabIndex = 0;
            GamePathLabel.Text = "Game Path:";
            GamePathTextBox.Location = new Point(120, 132);
            GamePathTextBox.Name = "GamePathTextBox";
            GamePathTextBox.Size = new Size(185, 23);
            GamePathTextBox.TabIndex = 0;
            BrowseGameButton.Location = new Point(310, 131);
            BrowseGameButton.Name = "BrowseGameButton";
            BrowseGameButton.Size = new Size(70, 23);
            BrowseGameButton.TabIndex = 0;
            BrowseGameButton.Text = "Browse";
            BrowseGameButton.Click += BrowseGameButton_Click;
            ModDataLabel.Location = new Point(12, 170);
            ModDataLabel.Size = new Size(100, 23);
            ModDataLabel.Text = "ModData:";
            ModDataComboBox.Location = new Point(120, 166);
            ModDataComboBox.Size = new Size(185, 23);
            ModDataComboBox.DropDownStyle = ComboBoxStyle.DropDownList;
            ModDataComboBox.Enabled = false;
            ModDataCheckBox.Location = new Point(310, 168);
            ModDataCheckBox.Size = new Size(75, 23);
            ModDataCheckBox.Text = "Enabled";
            ModDataCheckBox.CheckedChanged += ModDataCheckBox_CheckedChanged;
            ArgsLabel.Location = new Point(12, 205);
            ArgsLabel.Name = "ArgsLabel";
            ArgsLabel.Size = new Size(100, 23);
            ArgsLabel.TabIndex = 7;
            ArgsLabel.Text = "Launch Args:";
            ArgsTextBox.Location = new Point(120, 203);
            ArgsTextBox.Name = "ArgsTextBox";
            ArgsTextBox.Size = new Size(260, 23);
            ArgsTextBox.TabIndex = 8;
            LaunchButton.Location = new Point(12, 246);
            LaunchButton.Name = "LaunchButton";
            LaunchButton.Size = new Size(396, 47);
            LaunchButton.TabIndex = 9;
            LaunchButton.Text = "Launch";
            LaunchButton.Click += LaunchButton_Click;
            GW1Radio.Text = "GW1";
            GW1Radio.Location = new Point(20, 0);
            GW1Radio.Size = new Size(50, 20);
            GW1Radio.Checked = true;
            GW1Radio.CheckedChanged += GameRadioChanged;
            GW2Radio.Text = "GW2";
            GW2Radio.Location = new Point(90, 0);
            GW2Radio.CheckedChanged += GameRadioChanged;

            if (IsDarkMode())
            {

                ModDataComboBox.DrawMode = DrawMode.OwnerDrawFixed;
                ModDataComboBox.DrawItem += ModDataComboBox_DrawItem;
                UsernameTextBox.BorderStyle = BorderStyle.None;
                ServerIpTextBox.BorderStyle = BorderStyle.None;
                ArgsTextBox.BorderStyle = BorderStyle.None;
                LaunchButton.FlatStyle = FlatStyle.Flat;
                GamePathTextBox.BorderStyle = BorderStyle.None;
                BrowseGameButton.FlatStyle = FlatStyle.Flat;
                BrowseGameButton.FlatAppearance.BorderSize = 0;
                ModDataComboBox.FlatStyle = FlatStyle.Flat;

            }

            ClientSize = new Size(420, 305);
            Controls.Add(GamePathLabel);
            Controls.Add(GamePathTextBox);
            Controls.Add(BrowseGameButton);
            Controls.Add(UsernameLabel);
            Controls.Add(UsernameTextBox);
            Controls.Add(ServerIpLabel);
            Controls.Add(ServerIpTextBox);
            Controls.Add(ServerPasswordLabel);
            Controls.Add(ServerPasswordTextBox);
            Controls.Add(ModDataLabel);
            Controls.Add(ModDataComboBox);
            Controls.Add(ModDataCheckBox);
            Controls.Add(ArgsLabel);
            Controls.Add(ArgsTextBox);
            Controls.Add(LaunchButton);
            Controls.Add(GW1Radio);
            Controls.Add(GW2Radio);
            Text = "Launcher";
            ResumeLayout(false);
            PerformLayout();
            this.FormBorderStyle = FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
        }
    }
}
