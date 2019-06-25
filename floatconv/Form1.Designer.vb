<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'フォームがコンポーネントの一覧をクリーンアップするために dispose をオーバーライドします。
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        Try
            If disposing AndAlso components IsNot Nothing Then
                components.Dispose()
            End If
        Finally
            MyBase.Dispose(disposing)
        End Try
    End Sub

    'Windows フォーム デザイナーで必要です。
    Private components As System.ComponentModel.IContainer

    'メモ: 以下のプロシージャは Windows フォーム デザイナーで必要です。
    'Windows フォーム デザイナーを使用して変更できます。  
    'コード エディターを使って変更しないでください。
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.Tex1 = New System.Windows.Forms.TextBox()
        Me.Button1 = New System.Windows.Forms.Button()
        Me.Pic1 = New System.Windows.Forms.PictureBox()
        Me.Button2 = New System.Windows.Forms.Button()
        Me.Text2 = New System.Windows.Forms.TextBox()
        CType(Me.Pic1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'Tex1
        '
        Me.Tex1.Location = New System.Drawing.Point(12, 12)
        Me.Tex1.Name = "Tex1"
        Me.Tex1.Size = New System.Drawing.Size(280, 19)
        Me.Tex1.TabIndex = 0
        Me.Tex1.Text = "..\..\data\0_1.png"
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(298, 12)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(42, 23)
        Me.Button1.TabIndex = 1
        Me.Button1.Text = "変換"
        Me.Button1.UseVisualStyleBackColor = True
        '
        'Pic1
        '
        Me.Pic1.Location = New System.Drawing.Point(12, 76)
        Me.Pic1.Name = "Pic1"
        Me.Pic1.Size = New System.Drawing.Size(280, 280)
        Me.Pic1.TabIndex = 2
        Me.Pic1.TabStop = False
        '
        'Button2
        '
        Me.Button2.Location = New System.Drawing.Point(299, 41)
        Me.Button2.Name = "Button2"
        Me.Button2.Size = New System.Drawing.Size(41, 23)
        Me.Button2.TabIndex = 3
        Me.Button2.Text = "確認"
        Me.Button2.UseVisualStyleBackColor = True
        '
        'Text2
        '
        Me.Text2.Location = New System.Drawing.Point(12, 41)
        Me.Text2.Name = "Text2"
        Me.Text2.Size = New System.Drawing.Size(280, 19)
        Me.Text2.TabIndex = 4
        Me.Text2.Text = "..\..\data\0_1.bin"
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 12.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(347, 368)
        Me.Controls.Add(Me.Text2)
        Me.Controls.Add(Me.Button2)
        Me.Controls.Add(Me.Pic1)
        Me.Controls.Add(Me.Button1)
        Me.Controls.Add(Me.Tex1)
        Me.Name = "Form1"
        Me.Text = "Form1"
        CType(Me.Pic1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

    Friend WithEvents Tex1 As TextBox
    Friend WithEvents Button1 As Button
    Friend WithEvents Pic1 As PictureBox
    Friend WithEvents Button2 As Button
    Friend WithEvents Text2 As TextBox
End Class
