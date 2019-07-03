Option Explicit On
Imports System
Imports System.IO
Imports System.Drawing.Imaging

Imports System.Windows.Forms
Imports System.Windows.Media
Imports System.Windows.Media.Imaging

Public Class Form1
    Dim imgbf As Byte() = New Byte(640 * 480 + 1024) {}
    Dim runf = 0, buttondef = 0, dmode = 4, bmofs = 15, bmwidth, bmheight
    Dim timeron = 0, keyold = 255
    Dim fileno
    Private Sub disprgb(rcvdata, rcvsize)
        Dim p, po, r, g, b, h, l, xp, yp, ofs, dw, dh As Integer
        Dim c As System.Drawing.Color
        dw = bmwidth : dh = bmheight : ofs = bmofs
        Dim img As New Bitmap(dw, dh)
        For yp = 0 To bmheight - 1
            For xp = 0 To bmwidth - 1
                po = (yp * dw + xp) * 2 + ofs
                h = rcvdata(po) : l = rcvdata(po + 1)
                r = h And &HF8
                g = ((h << 5) And &HE0) Or ((l >> 3) And &H1C)
                b = (l << 3) And &HF8
                c = System.Drawing.Color.FromArgb(r, g, b)
                img.SetPixel(xp, yp, c)
            Next xp
        Next yp
        PictureBox1.Image = img
        img = Nothing
    End Sub
    Private Sub dispmono(rcvdata, rcvsize)
        Dim p, po, r, g, b, xp, yp, ofs, dw, dh As Integer
        Dim c As System.Drawing.Color
        dw = bmwidth : dh = bmheight : ofs = bmofs
        Dim img As New Bitmap(dw, dh)
        For yp = 0 To bmheight - 1
            For xp = 0 To bmwidth - 1
                po = yp * dw + xp + ofs
                r = rcvdata(po) ' Dim g = r : Dim b = r
                c = System.Drawing.Color.FromArgb(r, r, r)
                img.SetPixel(xp, yp, c)
            Next xp
        Next yp
        PictureBox1.Image = img
        Dim keytrig
        Dim keysts = rcvdata(13)
        If (keyold = 53 And keysts = 52) Or CheckBox3.Checked = False Then keytrig = 1 Else keytrig = 0
        keyold = keysts
        If keytrig = 1 Then savefile() ' 有効時かKey.Push時のみ格納処理
        img = Nothing
    End Sub

    Private Sub dispbayer(rcvdata, rcvsize)
        Dim p, po, ptn, r, g, b, xp, yp, ofs, dw, dh As Integer
        Dim c As System.Drawing.Color
        dw = bmwidth : dh = bmheight : ofs = bmofs
        Dim img As New Bitmap(dw, dh)
        For yp = 0 To bmheight - 1
            For xp = 0 To bmwidth - 1
                po = yp * dw + xp + ofs
                ptn = (yp And 1) * 2 + (xp And 1)
                Select Case ptn
                    Case 0 ' GBRG
                        r = rcvdata(po + dw) : g = rcvdata(po) : b = rcvdata(po + 1)
                    Case 1 ' BGGR
                        r = rcvdata(po + dw + 1) : g = rcvdata(po + 1) : b = rcvdata(po)
                    Case 2 ' RGGB
                        r = rcvdata(po) : g = rcvdata(po + 1) : b = rcvdata(po + dw + 1)
                    Case 3 ' GRBG
                        r = rcvdata(po + 1) : g = rcvdata(po) : b = rcvdata(po + dw)
                End Select
                c = System.Drawing.Color.FromArgb(r, g, b)
                img.SetPixel(xp, yp, c)
            Next xp
        Next yp
        PictureBox1.Width = dw : PictureBox1.Height = dh
        PictureBox1.Image = img
        img = Nothing
    End Sub

    Private Sub dispjpeg(rcvdata, rcvsize)
        ' 表示
        Dim Stream As New MemoryStream(imgbf, False)
        Dim img As System.Drawing.Image = System.Drawing.Image.FromStream(Stream)
        PictureBox1.Width = bmwidth : PictureBox1.Height = bmheight
        PictureBox1.Image = img
        savefile()
        img = Nothing
    End Sub

    Private Sub savefile()
        'ファイルに書き込む
        If CheckBox1.Checked = True Then
            Dim subdir As String = TextBox8.Text + "\" + TextBox9.Text
            If subdir <> "\" Then
                If Directory.Exists(subdir) = False Then Directory.CreateDirectory(subdir)
                subdir = subdir + "\"
            Else
                subdir = ""
            End If
            Dim fileName As String = subdir + TextBox2.Text '保存先のファイル名を作成
            Dim fext = Microsoft.VisualBasic.Right(fileName, 4)
            If CheckBox2.Checked = True Then ' 連番処理、拡張子が3桁を仮定
                fileName = Microsoft.VisualBasic.Left(fileName, Len(fileName) - 4) + CStr(fileno) + fext
                fileno = fileno + 1 : TextBox7.Text = CStr(fileno)
            End If
            If (fext = ".jpg") Then PictureBox1.Image.Save(fileName, System.Drawing.Imaging.ImageFormat.Jpeg)
            If (fext = ".png") Then png_out(fileName, PictureBox1)
        End If

    End Sub
    Private Sub get_http(url As String)
        Dim imgbfp = 0
        'WebRequestを作成
        Dim webreq As System.Net.HttpWebRequest = DirectCast(System.Net.WebRequest.Create(url), System.Net.HttpWebRequest)
        webreq.Timeout = 2000
        Try
            Dim webres As System.Net.HttpWebResponse = DirectCast(webreq.GetResponse(), System.Net.HttpWebResponse)
            Dim strm As System.IO.Stream = webres.GetResponseStream()
            Dim readData As Byte() = New Byte(1023) {}
            While True
                Dim readSize As Integer = strm.Read(readData, 0, readData.Length)
                If readSize = 0 Then Exit While
                Buffer.BlockCopy(readData, 0, imgbf, imgbfp, readSize)
                imgbfp = imgbfp + readSize
                If runf = 0 Then strm.Close() : Exit Sub ' Stop Run
            End While
            strm.Close()
            ' 表示
            If dmode = 1 Then dispmono(imgbf, imgbfp)
            If dmode = 2 Then dispjpeg(imgbf, imgbfp)
            If dmode = 3 Then disprgb(imgbf, imgbfp)
            If dmode = 4 Then dispbayer(imgbf, imgbfp)
        Catch
            Button1.BackColor = System.Drawing.Color.Red
            Application.DoEvents()
            System.Threading.Thread.Sleep(1000)
            Exit Sub
        End Try

    End Sub
    Private Sub csv_out(maindir)
        Dim trause, tract
        Dim files As String() = System.IO.Directory.GetFiles(maindir, "*", System.IO.SearchOption.AllDirectories)
        Dim fmax = files.Length
        Dim ary As Integer() : ReDim ary(fmax - 1)
        Dim i : For i = 0 To fmax - 1 : ary(i) = i : Next
        Dim ary2 As Integer() = ary.OrderBy(Function(j) Guid.NewGuid()).ToArray() 'シャッフルする 
        Dim file1 As StreamWriter = New StreamWriter(maindir + "_training.csv", False) : file1.WriteLine("x:image,y:label")
        Dim file2 As StreamWriter = New StreamWriter(maindir + "_test.csv", False) : file2.WriteLine("x:image,y:label")
        trause = Val(TextBox10.Text) ' training xx , test 1 の割合で作成
        tract = 0
        For i = 0 To fmax - 1
            Dim fn, cp1, cp2, dn
            fn = files(ary2(i))
            cp1 = InStr(fn, "\") : cp2 = InStr(cp1 + 1, fn, "\") : dn = Mid(fn, cp1 + 1, cp2 - cp1 - 1)
            If trause <> tract Then
                file1.WriteLine(fn + "," + dn)
            Else
                file2.WriteLine(fn + "," + dn)
                tract = 0
            End If
            tract = tract + 1
        Next
        file1.Close() : file2.Close()
    End Sub
    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        csv_out(TextBox8.Text)
    End Sub

    Private Sub Button3_Click(sender As Object, e As EventArgs) Handles Button3.Click
        runf = 0
        End
    End Sub

    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        Dim uext
        Dim wtime = Val(TextBox6.Text)
        If runf = 0 Then
            runf = 1 : fileno = TextBox7.Text
            If RadioButton1.Checked = True Then dmode = 1 : uext = "pgm"
            If RadioButton2.Checked = True Then dmode = 2 : uext = "jpg"
            If RadioButton3.Checked = True Then dmode = 3
            If RadioButton4.Checked = True Then dmode = 4
            If RadioButton5.Checked = True Then
                bmwidth = 160 : bmheight = 120
            Else
                bmwidth = 320 : bmheight = 240
            End If
            PictureBox1.Width = bmwidth : PictureBox1.Height = bmheight
            TextBox1.Text = Microsoft.VisualBasic.Left(TextBox1.Text, Len(TextBox1.Text) - 3) + uext
            Button1.Text = "中断"
            Timer1.Interval = wtime
            Timer1.Enabled = True
        Else
            runf = 0
            Timer1.Enabled = False
            Button1.BackColor = buttondef
            Button1.Text = "再開"
        End If
    End Sub

    Private Sub Timer1_Tick(sender As Object, e As EventArgs) Handles Timer1.Tick
        If timeron = 0 Then
            timeron = 1
            Button1.BackColor = System.Drawing.Color.LightGreen : Application.DoEvents()
            get_http(TextBox1.Text)
            Button1.BackColor = buttondef : Application.DoEvents()
            timeron = 0
        End If
    End Sub

    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        Label3.Text = System.Environment.CurrentDirectory + "\"
        buttondef = Button1.BackColor
    End Sub

    Private Sub png_out(filename As String, Pic As PictureBox)
        Dim Width = 28
        Dim Height = 28
        Dim h, w, cg
        ' 28x28に近いサイズまで縮小
        Dim canvas As New Bitmap(PictureBox2.Width, PictureBox2.Height)
        Dim g As Graphics = Graphics.FromImage(canvas)
        g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.NearestNeighbor ' 最近傍補間
        'g.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic ' 双三次補間
        g.DrawImage(Pic.Image, 0, 0, 40, 30)
        PictureBox2.Image = canvas
        ' クレースケール化
        Dim P As Bitmap = CType(PictureBox2.Image, Bitmap)
        Dim c As System.Drawing.Color
        Dim bmpdata As Byte() = New Byte(Width * Height) {}
        For h = 0 To Height - 1
            For w = 0 To Width - 1
                c = P.GetPixel(w + 6, h + 1)
                cg = (30 * c.R + 59 * c.G + 11 * c.B) / 100 ' 標準テレビジョン放送規格に準拠(らしい)
                bmpdata(h * Height + w) = cg
                c = System.Drawing.Color.FromArgb(cg, cg, cg)
                P.SetPixel(w + 6, h + 1, c)
            Next
        Next
        PictureBox3.Image = PictureBox2.Image
        ' 8ビットPNGで書き出し
        Dim image As BitmapSource = System.Windows.Media.Imaging.BitmapSource.Create(
                Width, Height, 96, 96, ' DPI x,y
                PixelFormats.Indexed8, BitmapPalettes.Gray256, bmpdata, Width)
        Dim Stream As FileStream = File.Create(filename)
        Dim encoder As New PngBitmapEncoder()
        encoder.Interlace = PngInterlaceOption.On
        encoder.Frames.Add(BitmapFrame.Create(image))
        encoder.Save(Stream)
        image = Nothing : Stream.Close() : encoder = Nothing
        Application.DoEvents()
    End Sub



End Class
