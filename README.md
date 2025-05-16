# RichString
To process rich text in QT.

**Think of this:** 
You have a **QPixmap**, you need to **draw some text** onto it, and it needs to
**wrap automatically**. The general idea would be calling ``` painter.drawText(pix.rect(), Qt::AlignCenter | Qt::Qt::TextWrapAnywhere, str);```,
which works fine when every each character has the **same font and color**, but when that varies, this method doesn't work. 

Then you think of
**dividing the block** into pieces, and calculate them **separately**, which will involve **calculating each character in the text**, 
what position they should be at, and how big they should look like on the paint device, which **will then** lead to a **MINOR HEADACHE**.
Well that sucks, tedious work that ruins the day.

Check this tool out, save some energy for a **great after-work meal**.