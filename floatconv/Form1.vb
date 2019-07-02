Imports System.Drawing
Public Class Form1
    Dim ByteArray() As Byte
    Private Sub getbf()
        Dim fname As String = Text2.Text
        Dim fs As New System.IO.FileStream(fname, System.IO.FileMode.Open, System.IO.FileAccess.Read)
        ReDim ByteArray(fs.Length - 1)
        fs.Read(ByteArray, 0, ByteArray.Length)
        fs.Close()
    End Sub
    Private Sub dispbmp()
        Dim canvas As New Bitmap(Pic1.Width, Pic1.Height)
        Dim g As Graphics = Graphics.FromImage(canvas)
        Dim xs, ys, xp, yp, c
        Dim fd As Single
        For ys = 0 To 27
            For xs = 0 To 27
                fd = BitConverter.ToSingle(ByteArray, (ys * 28 + xs) * 4)
                yp = ys * 10
                xp = xs * 10
                c = fd And 255
                Dim b As New SolidBrush(Color.FromArgb(255, c, c, c))
                g.FillRectangle(b, xp, yp, xp + 10, yp + 10)
                b.Dispose()
            Next
        Next
        g.Dispose()
        Pic1.Image = canvas
    End Sub
    Private Sub cnvp2s()
        Dim canvas As New Bitmap(Pic1.Width, Pic1.Height)
        Dim g As Graphics = Graphics.FromImage(canvas)

        Dim img As Image = Image.FromFile(Text1.Text)
        Pic1.Image = img
        Dim P As Bitmap
        Dim bc As Color
        Dim sc As Single
        P = CType(Pic1.Image, Bitmap)

        Dim fname As String = Text2.Text
        Dim fs As New System.IO.FileStream(fname,
    System.IO.FileMode.Create,
    System.IO.FileAccess.Write)
        ReDim ByteArray(3)

        Dim xs, ys, xp, yp, c
        For ys = 0 To 27
            For xs = 0 To 27
                yp = ys * 10
                xp = xs * 10
                bc = P.GetPixel(xs, ys)
                c = bc.ToArgb() And 255
                sc = c
                ByteArray = BitConverter.GetBytes(sc)
                fs.Write(ByteArray, 0, 4)
                Dim b As New SolidBrush(Color.FromArgb(255, c, c, c))
                g.FillRectangle(b, xp, yp, xp + 10, yp + 10)
                b.Dispose()
            Next
        Next
        Pic1.Image = canvas
        fs.Close()

    End Sub
    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        cnvp2s()
    End Sub

    Private Sub Button2_Click(sender As Object, e As EventArgs) Handles Button2.Click
        getbf()
        dispbmp()
    End Sub

    Private Sub Button3_Click(sender As Object, e As EventArgs) Handles Button3.Click
        Dim files As String() = System.IO.Directory.GetFiles(Text3.Text, "*", System.IO.SearchOption.AllDirectories)
        Dim fmax = files.Length
        Dim i, pfn, bfn
        For i = 0 To fmax - 1
            pfn = files(i)
            If Microsoft.VisualBasic.Right(pfn, 3) = "png" Then
                bfn = Mid(pfn, 1, Len(pfn) - 3) + "bin"
                Text1.Text = pfn
                Text2.Text = bfn
                cnvp2s()
                Application.DoEvents()
            End If
        Next
    End Sub

    Private Sub Button4_Click(sender As Object, e As EventArgs) Handles Button4.Click
        End
    End Sub
End Class
