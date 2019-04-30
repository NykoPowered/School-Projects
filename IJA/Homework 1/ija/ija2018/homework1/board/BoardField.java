/* IJA ukol 1 2019
    @author Nikolas Masica
 */

package ija.ija2018.homework1.board;

public class BoardField implements Field
{
    private int row;
    private int col;
    private Disk disk;
    private Field[] this_field = new Field[8];

    public BoardField(int col, int row)
    {
        this.row = row;
        this.col = col;
    }

    public void addNextField(Field.Direction dirs, Field field)
    {
        if (dirs == Field.Direction.D)
        {
            this_field[0]=field;
        }
        else if (dirs == Field.Direction.L)
        {
            this_field[1]=field;
        }
        else if (dirs == Field.Direction.LD)
        {
            this_field[2]=field;
        }
        else if (dirs == Field.Direction.LU)
        {
            this_field[3]=field;
        }
        else if (dirs == Field.Direction.R)
        {
            this_field[4]=field;
        }
        else if (dirs == Field.Direction.RD)
        {
            this_field[5]=field;
        }
        else if (dirs == Field.Direction.RU)
        {
            this_field[6]=field;
        }
        else if (dirs == Field.Direction.U)
        {
            this_field[7]=field;
        }
    }

    public Field nextField(Field.Direction dirs)
    {
        if (dirs == Field.Direction.D)
        {
            return this_field[0];
        }
        else if (dirs == Field.Direction.L)
        {
            return this_field[1];
        }
        else if (dirs == Field.Direction.LD)
        {
            return this_field[2];
        }
        else if (dirs == Field.Direction.LU)
        {
            return this_field[3];
        }
        else if (dirs == Field.Direction.R)
        {
            return this_field[4];
        }
        else if (dirs == Field.Direction.RD)
        {
            return this_field[5];
        }
        else if (dirs == Field.Direction.RU)
        {
            return this_field[6];
        }
        else if (dirs == Field.Direction.U)
        {
            return this_field[7];
        }
        else {
            return null;
        }
    }



    public boolean put(Disk disk)
    {
        if(this.disk == null){
            disk.position(this);
            this.disk = disk;
            return true;
        }else{
            return false;
        }


    }

    public Disk get()
    {
        if (disk == null)
        {
            return null;
        }
        else
        {
            return disk;
        }
    }

    public boolean isEmpty()
    {
        if(this.disk == null){
            return true;
        }
        else{
            return false;
        }
    }

    public int getrow(){
        return this.row;
    }

    public int getcol(){
        return this.col;
    }

    public boolean remove(Disk disk)
    {

        if(this.disk == disk) {
            disk.position(null);
            this.disk = null;
            return true;

        }
        else{
            return false;
        }
    }
}
