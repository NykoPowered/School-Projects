/* IJA ukol 1 2019
    @author Nikolas Masica
 */

package ija.ija2018.homework1.board;

public class Disk{

    private boolean isWhite;
    private Field coordinates;

    public Disk(boolean isWhite)
    {
        this.isWhite = isWhite;
    }

    public boolean isWhite()
    {
        if(isWhite)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    public void position(Field field){
        this.coordinates = field;
    }

    public boolean move(Field moveTo)
    {
        Field help = this.coordinates;

        if(this.coordinates.getrow() != moveTo.getrow() && this.coordinates.getcol() != moveTo.getcol()){
            return false;
        }else{
            if (this.coordinates.getrow() == moveTo.getrow()) {
                if (this.coordinates.getcol() > moveTo.getcol()) {
                    while(help != moveTo){
                        if(!help.nextField(Field.Direction.L).isEmpty()){
                            return false;
                        }
                        else{
                            help = help.nextField(Field.Direction.L);
                        }
                    }
                    this.coordinates.remove(this);
                    moveTo.put(this);
                    return true;
                }else if(this.coordinates.getcol() < moveTo.getcol()){
                    while(help != moveTo){
                        if(!help.nextField(Field.Direction.R).isEmpty()){
                            return false;
                        }
                        else{
                            help = help.nextField(Field.Direction.R);
                        }
                    }
                    this.coordinates.remove(this);
                    moveTo.put(this);
                    return true;
                }
            }
            else if(this.coordinates.getcol() == moveTo.getcol()){
                if (this.coordinates.getrow() > moveTo.getrow()){
                    while(help != moveTo){
                        if(!help.nextField(Field.Direction.D).isEmpty()){
                            return false;
                        }
                        else{
                            help = help.nextField(Field.Direction.D);
                        }
                    }
                    this.coordinates.remove(this);
                    moveTo.put(this);
                    return true;
                }else if(this.coordinates.getrow() < moveTo.getrow()){
                    while(help != moveTo){
                        if(!help.nextField(Field.Direction.U).isEmpty()){
                            return false;
                        }
                        else{
                            help = help.nextField(Field.Direction.U);
                        }
                    }
                    this.coordinates.remove(this);
                    moveTo.put(this);
                    return true;
                }
            }
        }
        return true;
    }

}

