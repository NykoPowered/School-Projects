/* IJA ukol 1 2019
    @author Nikolas Masica
 */

package ija.ija2018.homework1.board;

public interface Field{
    public static enum Direction
    {
        D,L,LD,LU,R,RD,RU,U
    }

    public void addNextField(Field.Direction dirs,Field field);
    public Disk get();
    public boolean isEmpty();
    public Field nextField(Field.Direction dirs);
    public boolean put(Disk disk);
    public boolean remove(Disk disk);
    public int getrow();
    public int getcol();


}