/* IJA ukol 1 2019
    @author Nikolas Masica
 */

package ija.ija2018.homework2.common;

public interface Field{
    public static enum Direction
    {
        D,L,LD,LU,R,RD,RU,U
    }

    public void addNextField(Field.Direction dirs,Field field);
    public boolean isEmpty();
    public Figure get();
    public boolean put(Figure figure);
    public boolean remove(Figure figure);
    public Field nextField(Field.Direction dirs);
    public int getrow();
    public int getcol();



}