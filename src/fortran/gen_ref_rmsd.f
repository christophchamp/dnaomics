
	program roteuler2

	implicit double precision (a-h,o-z)
        integer      NRES,ATM_RES,PROTN,NMOL
        parameter    (NRES=1300,ATM_RES=20,PROTN=29,NMOL=10)
        double precision         CCELEC
        parameter    (CCELEC=332.0716)

c rtf arrays
        character*4  am_acid(PROTN)
        character*4  atomname(ATM_RES,PROTN),atomtype(ATM_RES,PROTN)
        character*4  bond1(ATM_RES,PROTN),bond2(ATM_RES,PROTN)
        integer      atomnum(PROTN),bondnum(PROTN)
	double precision charge(ATM_RES,PROTN),rcharge(PROTN)
        double precision atomrad(ATM_RES,PROTN)
        common /param1/ am_acid,atomname,atomtype,bond1,bond2
        common /param2/ atomnum,bondnum,charge,rcharge,atomrad

c pdb 1
        integer       res_num1,nm1,atm_num1,res1_id(NRES)
        character*4   res1_name(NRES)
        character*4   res1_atmname(ATM_RES,NRES)
        integer       res1_atmnum(NRES)
        double precision res1_atmcoord(ATM_RES,NRES,3)
        double precision res1_atmrad(ATM_RES,NRES)
        double precision res1_charge(NRES)
        double precision res1_atmcharge(ATM_RES,NRES)
        integer       res1_segcnt(NRES)
        integer       mol1(2,NMOL)
        character*4   res1_segid(NRES)
        character*4   segid1(NMOL)

c pdb 2
        integer       res_num2,nm2,atm_num2,res2_id(NRES)
        character*4   res2_name(NRES)
        character*4   res2_atmname(ATM_RES,NRES)
        integer       res2_atmnum(NRES)
        double precision res2_atmcoord(ATM_RES,NRES,3)
        double precision res2_atmrad(ATM_RES,NRES)
        double precision res2_charge(NRES)
        double precision res2_atmcharge(ATM_RES,NRES)
        integer       res2_segcnt(NRES)
        integer       mol2(2,NMOL)
        character*4   res2_segid(NRES)
        character*4   segid2(NMOL)

        character*80 ARG
        integer NARG
        double precision eps

c angles of rotation and rank of closest dist ligand receptor 
	real rad1(11999),rad2(4700)
        double precision chg(4700),chg1(11999)
        logical saa(4700),saa1(11999),check(4700)
        logical saat(4700),saat1(11999)
 	integer ibound(0:200),resid(4700)

c matrixes 
        double precision t1(3),t2(3)

c The distance to place the receptor and the ligand
        double precision d_rl

c the coordinates of receptor and ligand after being centered
c at their center of mass
       double precision cc1(ATM_RES,NRES,3), cc2(ATM_RES,NRES,3)
       double precision cc(ATM_RES,NRES,3),ccl(ATM_RES,NRES,3)

c the coordinates of receptor and ligand after each rotation

       double precision xx1(11999),yy1(11999),zz1(11999)
       double precision xx2(4700),yy2(4700),zz2(4700)
       double precision xxl(11999),yyl(11999),zzl(11999)
       double precision xl1(4700),yl1(4700),zl1(4700)
       double precision xl2(4700),yl2(4700),zl2(4700)
       integer atm1(11999),atm2(4700)
	
	real res_scarea(NRES),res_area(NRES),res_atmarea(ATM_RES,NRES)
	real coord(ATM_RES,NRES,3), radii(ATM_RES,NRES)

c dist: the distance between the center of mass of two molecules
       double precision d2, dist
	common/ang/dcthe,dsthe,dcphi,dsphi

        NARG=iargc()
        if(NARG.ne.2) then
c	Added next line by Christoph Champ, 22-Oct-2004
	  print*, 'ScreenDOT'
          print*, 'USAGE: screendot.x ',
     *	  'prot1 output'
          stop
        end if
        call getarg(1,ARG)
        open(unit=1,file=ARG,status='old')
        call getarg(2,ARG)
        open(unit=11,file=ARG,status='unknown')

        call  getrtf(am_acid,atomname,atomtype,bond1,bond2,
     &                 atomnum,bondnum,charge,rcharge,atomrad)

        call readin(1,res_num1,atm_num1,res1_id,res1_name,res1_atmname
     &    ,res1_atmnum,res1_atmcoord,res1_atmrad,res1_charge,
     &    res1_atmcharge,res1_segid,res1_segcnt,nm1,mol1,segid1)

        call center(res_num1,res1_atmnum,res1_atmname,
     &              res1_atmcoord,t1)

        k=0
        do i = 1, res_num1
           do j = 1, res1_atmnum(i)
	      cc1(j,i,1)=res1_atmcoord(j,i,1)
	      cc1(j,i,2)=res1_atmcoord(j,i,2)
	      cc1(j,i,3)=res1_atmcoord(j,i,3)
              k = k+1
           if (res1_atmname(j,i).eq.'OCT1') then
                write(11,10) 
     &            cc1(j,i,1),cc1(j,i,2),cc1(j,i,3)
           else if (res1_atmname(j,i).eq.'OCT2') then
                write(11,10) 
     &            cc1(j,i,1),cc1(j,i,2),cc1(j,i,3)
           else
                write(11,10) 
     &            cc1(j,i,1),cc1(j,i,2),cc1(j,i,3)
10              FORMAT(3(F8.3,1x))
           end if
           end do
        end do
	close(11)

        stop
        end

c ***********************************************************************
c Parameters of amino acids 
c used for checking the completeness of the pdb sequences
c ***********************************************************************
      SUBROUTINE getrtf(am_acid,atomname,atomtype,bond1,bond2,
     &                 atomnum,bondnum,charge,rcharge,atomrad)
c =======================================================================
c  am_acid():     the names of the amino acids
c  atomnum():     the number of atoms in a amino acid
c  atomname()():  the names of atoms in a amino acid
c  charge():      the charge of each atom
c  rcharge():     the total charge of a residue
c  bondnum():     the number of bonds formed within this residue
c  bond1():       atom 1 of the bond
c  bond2():       atom 2 of the bond
c========================================================================

        integer      ATM_RES,PROTN
        PARAMETER    (ATM_RES=20,PROTN=29)

        character*80 line

        character*4  am_acid(PROTN)
        character*4  atomname(ATM_RES,PROTN),atomtype(ATM_RES,PROTN)
        character*4  bond1(ATM_RES,PROTN),bond2(ATM_RES,PROTN)
        integer      atomnum(PROTN),bondnum(PROTN)
	double precision charge(ATM_RES,PROTN),rcharge(PROTN)
        double precision atomrad(ATM_RES,PROTN)


        open(unit=10,
     &       file='/home/structure/lib/smpl.rtf',
     &       status='old')

        ir=0
1       read(10,'(A80)',end=2) line
        if(line(1:4).eq.'RESI') then
           if(ir.ne.0) then
              atomnum(ir)=ia
              bondnum(ir)=ib
           end if
           ia=0
           ib=0
           ir=ir+1
           read(line,'(5x,a4,8x,f5.2)') am_acid(ir),rcharge(ir)
        else if(line(1:4).eq.'ATOM') then
           ia=ia+1
           read(line,3) atomname(ia,ir)
     &         ,atomtype(ia,ir),charge(ia,ir),atomrad(ia,ir)
3          format(5x,a4,1x,a4,3x,f5.2,3x,f6.3)
        else if(line(1:4).eq.'BOND') then
           ib=ib+1
           read(line,'(5x,a4,1x,a4)') bond1(ib,ir),bond2(ib,ir)
        else 
           goto 1
        end if
        goto 1

2       if(ir.ne.PROTN) stop 'Error getrtf'
        atomnum(ir)=ia
        bondnum(ir)=ib

        return 
        end


c ***********************************************************************
c Find out the center of mass of a molecule 
c***********************************************************************
      SUBROUTINE center(nr,res_atmnum,res_atmname,res_atmcoord,
     &                  t)

        integer      NRES,ATM_RES
        parameter    (NRES=1300,ATM_RES=20)

        integer       nr
        character*4   res_atmname(ATM_RES,NRES)
        integer       res_atmnum(NRES)
        double precision res_atmcoord(ATM_RES,NRES,3), t(3) 
        double precision summ(3), sumc(3), m

        summ(1) = 0.d0
        summ(2) = 0.d0
        summ(3) = 0.d0
        sumc(1) = 0.d0
        sumc(2) = 0.d0
        sumc(3) = 0.d0
        do 100 ir = 1, nr
           do 100 ia = 1, res_atmnum(ir)

              if (res_atmname(ia,ir)(1:1) .eq. 'H') then
                 m = 1.d0
              else if (res_atmname(ia,ir)(1:1) .eq. 'C') then
                 m = 6.d0
                 m = 1.d0
              else if (res_atmname(ia,ir)(1:1) .eq. 'N') then
                 m = 7.d0
                 m = 1.d0
              else if (res_atmname(ia,ir)(1:1) .eq. 'O') then
                 m = 8.d0
                 m = 1.d0
              else if (res_atmname(ia,ir)(1:1) .eq. 'S') then
                 m = 16.d0
                 m = 1.d0
              else
                 print*, 'Unknown atom: ', res_atmname(ia,ir)
                 m = 0.d0
              end if 

              do 100 j = 1,3
                 summ(j) = summ(j) + m * res_atmcoord(ia,ir,j)
                 sumc(j) = sumc(j) + m
100           continue

        do j = 1,3
           t(j) = summ(j) / sumc(j)
        end do
	write(*,*)'cm',t(1),t(2),t(3)


        return
        end

c************************************************************************
c This subroutine puts the center of the first molecule at the 
c origin, and that of the second molecule on z axis
c----------------------------------------------------------------

        subroutine mvtocm(res_num1,res1_atmnum,res1_atmname,
     &   res1_atmcoord,t1,res_num2,res2_atmnum,res2_atmname,
     &   res2_atmcoord,t2,ctemp1,ctemp2)

	implicit real*8 (a-h,o-z)
        integer      NRES,ATM_RES
        parameter    (NRES=1300,ATM_RES=20)

        integer res_num1, res1_atmnum(NRES)
        integer res_num2, res2_atmnum(NRES)

        character*4   res1_atmname(ATM_RES,NRES)
        character*4   res2_atmname(ATM_RES,NRES)

        double precision res1_atmcoord(ATM_RES,NRES,3)
        double precision res2_atmcoord(ATM_RES,NRES,3)
        double precision t1(3), t2(3), t(3)
        double precision angl_y, angl_z 
        double precision ctemp1(ATM_RES,NRES,3)
        double precision ctemp2(ATM_RES,NRES,3)
	double precision t1new(3), t2new(3)

        do 100 ir = 1, res_num1
           do 100 ia = 1, res1_atmnum(ir)
              do 100 j = 1,3
              ctemp1(ia,ir,j) = res1_atmcoord(ia,ir,j)
100     continue     

        do 200 ir = 1, res_num2
           do 200 ia = 1, res2_atmnum(ir)
              do 200 j = 1,3
              ctemp2(ia,ir,j) = res2_atmcoord(ia,ir,j) 
200     continue  

        write(*,*) 'Centers of Mass after initial orient:'
        call center(res_num1,res1_atmnum,res1_atmname,ctemp1,t1new)
        call center(res_num2,res2_atmnum,res2_atmname,ctemp2,t2new)

        return
        end


c----------------------------------------------------------------
c This subroutine 
c 
c	call roteuler(res_num1,res1_atmnum,ctemp1,theta,phi,spin,cc1)
c----------------------------------------------------------------

        subroutine roteuler(res_num,res_atmnum,ctemp,th,ph,sp,cc)
        implicit real*8 (a-h,o-z)
        integer      NRES,ATM_RES
        parameter    (NRES=1300,ATM_RES=20)

        integer res_num, res_atmnum(NRES)

        double precision cc(ATM_RES,NRES,3)
        double precision ctemp(ATM_RES,NRES,3)

	dcthe=dcos(th)
	dsthe=dsin(th)
	dcphi=dcos(ph)
	dsphi=dsin(ph)
	dcspi=dcos(sp)
	dsspi=dsin(sp)
        do 200 ir = 1, res_num
           do 200 ia = 1, res_atmnum(ir)
           cc(ia,ir,1)=ctemp(ia,ir,1)*(dcphi*dcspi-dsphi*dcthe*dsspi)+
     &	               ctemp(ia,ir,2)*(-dcphi*dsspi-dsphi*dcthe*dcspi)+
     &		       ctemp(ia,ir,3)*dsthe*dsphi
           cc(ia,ir,2)=ctemp(ia,ir,1)*(dsphi*dcspi+dcphi*dcthe*dsspi)+
     &		       ctemp(ia,ir,2)*(-dsphi*dsspi+dcphi*dcthe*dcspi)-
     &		       ctemp(ia,ir,3)*dsthe*dcphi
           cc(ia,ir,3)=ctemp(ia,ir,1)*dsspi*dsthe+ctemp(ia,ir,2)*
     &		       dcspi*dsthe+ctemp(ia,ir,3)*dcthe
200     continue

	return
	end
	



        subroutine rotsurfeuler(xx,yy,zz,imax,th,ph,sp,x1,y1,z1)
        implicit real*8 (a-h,o-z)

        double precision xx(*),yy(*),zz(*),x1(*),y1(*),z1(*)

	dcthe=dcos(th)
	dsthe=dsin(th)
	dcphi=dcos(ph)
	dsphi=dsin(ph)
	dcspi=dcos(sp)
	dsspi=dsin(sp)
        do 200 ir = 1, imax
           x1(ir)=xx(ir)*(dcphi*dcspi-dsphi*dcthe*dsspi)+
     &	               yy(ir)*(-dcphi*dsspi-dsphi*dcthe*dcspi)+
     &		       zz(ir)*dsthe*dsphi
           y1(ir)=xx(ir)*(dsphi*dcspi+dcphi*dcthe*dsspi)+
     &		       yy(ir)*(-dsphi*dsspi+dcphi*dcthe*dcspi)-
     &		       zz(ir)*dsthe*dcphi
           z1(ir)=xx(ir)*dsspi*dsthe+yy(ir)*dcspi*dsthe
     &		       +zz(ir)*dcthe
200     continue

	return
	end





        subroutine pullsurfpolar(xx,yy,zz,imax,ddx,ddy,ddz,x1,y1,z1)
        implicit real*8 (a-h,o-z)

        double precision xx(*),yy(*),zz(*),x1(*),y1(*),z1(*)
	common/ang/dcthe,dsthe,dcphi,dsphi


	
        do 200 ir = 1, imax
              x1(ir) = xx(ir)+ddx
              y1(ir) = yy(ir)+ddy
              z1(ir) = zz(ir)+ddz
200     continue

	return
	end



c         call pullpolar(res_num2,res2_atmnum,cce,d_rl,theta,phi,cl)

        subroutine pullpolar(res_num,res_atmnum,ctemp,dl,theta,phi,cc)
        implicit real*8 (a-h,o-z)
        integer      NRES,ATM_RES
        parameter    (NRES=1300,ATM_RES=20)

        integer res_num, res_atmnum(NRES)

        double precision cc(ATM_RES,NRES,3)
        double precision ctemp(ATM_RES,NRES,3)

	dcthe=dcos(theta)
	dsthe=dsin(theta)
	dcphi=dcos(phi)
	dsphi=dsin(phi)

	x1=dl*dsthe*dcphi
	y1=dl*dsthe*dsphi
	z1=dl*dcthe
	
        do 200 ir = 1, res_num
           do 200 ia = 1, res_atmnum(ir)
              cc(ia,ir,1) = ctemp(ia,ir,1)+x1
              cc(ia,ir,2) = ctemp(ia,ir,2)+y1
              cc(ia,ir,3) = ctemp(ia,ir,3)+z1
200     continue

	return
	end



c************************************************************************
c This subroutine reads a pdb file and store the coordinates to arrays
c************************************************************************

       SUBROUTINE readin(iu,nr,atm_num,res_id,res_name,res_atmname,
     &    res_atmnum,res_atmcoord,res_atmrad,res_charge,res_atmcharge,
     &    res_segid,res_segcnt,nm,mol,segid)

c =======================================================================
c  iu:             the unit number of pdb file
c  res_name():     the name of a residue
c  res_atmnum():   the total # of each residue
c  res_atmname()():the names of atoms in a residue, get from res_id
c  res_atmcoord(): the coordinates of each atom in Cartesian Coordinates
c  res_atmcharge()(): the charge of each atom
c  res_charge():   the total charge of a residue
c  res_segid():    the segment id , 4 char code as in charmm
c  res_segcnt():   the count of a residue in a segment 
c -----------------------------------------------------------------------
c  nm:           the total # of molecules in the sequence
c  nr:           the total # of residues in the sequence 
c  atm_num:      the total # of atoms in the sequence    
c ======================================================================= 

        integer      NRES,ATM_RES,NMOL,PROTN
        parameter    (NRES=1300,ATM_RES=20,NMOL=10,PROTN=29)
  
        integer       iu,nr,nm,atm_num,res_id(NRES)
        character*4   res_name(NRES)
        character*4   res_atmname(ATM_RES,NRES)
        integer       res_atmnum(NRES)
        double precision res_atmcoord(ATM_RES,NRES,3)
        double precision res_atmrad(ATM_RES,NRES)
        double precision res_charge(NRES),res_atmcharge(ATM_RES,NRES)
        character*4   res_segid(NRES)
        integer       res_segcnt(NRES)
        integer       mol(2,NMOL)
        character*4   segid(NMOL)

        character*80  line
        integer       res_indx,atm_indx
        character*10  priv_res
        character*4   priv_seg
        logical       endfile
 
c       These arrays are used in the param common block
        character*4  am_acid(PROTN)
        character*4  atomname(ATM_RES,PROTN),atomtype(ATM_RES,PROTN)
        character*4  bond1(ATM_RES,PROTN),bond2(ATM_RES,PROTN)
        integer      atomnum(PROTN),bondnum(PROTN)
	double precision charge(ATM_RES,PROTN),rcharge(PROTN)
        double precision atomrad(ATM_RES,PROTN)
        common /param1/ am_acid,atomname,atomtype,bond1,bond2
        common /param2/ atomnum,bondnum,charge,rcharge,atomrad

c nr: the total number of residues
c na: the number of atoms in each residue

      nm=0
      nr = 0
      nsegr=0
      atm_num=0
      priv_seg='****'
      priv_res='STARTING  '
      endfile=.FALSE.

      do while(.true.)
	read(iu,'(A80)',end=1000) line
	if(line(1:4).ne.'ATOM') goto 999
c shift all the wrong atom name positions back
        if(line(13:16).eq.'HE21') line(13:17)=' HE21'
        if(line(13:16).eq.' NT ') line(13:17)=' N   '
        if(line(13:16).eq.'HE22') line(13:17)=' HE22'
        if(line(13:16).eq.'HD21') line(13:17)=' HD21'
        if(line(13:16).eq.'HD22') line(13:17)=' HD22'
        if(line(13:16).eq.'HH11') line(13:17)=' HH11'
        if(line(13:16).eq.'HH12') line(13:17)=' HH12'
        if(line(13:16).eq.'HH21') line(13:17)=' HH21'
        if(line(13:16).eq.'HH22') line(13:17)=' HH22'
        if(line(13:16).eq.'OCT1') line(13:17)=' OCT1'
        if(line(13:16).eq.'OCT2') line(13:17)=' OCT2'
        if(line(14:14).eq.'D')    line(14:14)='H'
        if(line(14:20).eq.'CD1 ILE') line(14:20)='CD  ILE'

	if(priv_res.ne.line(18:27)) then
C a new residue
           if(line(73:76).ne.priv_seg) then
C a new segment
              nm=nm+1
              nsegr=0
              mol(1,nm)=nr+1
              segid(nm)=line(73:76)
              priv_seg=line(73:76)
           end if

C clean up last residue
C----------------------------------------------------------------------
 10        if(nr.gt.0.and.na.ne.atomnum(res_indx)) 
     &       write(99,*) 'residue ',nr,' has ',(atomnum(res_indx)-na),
     &                ' atoms less than normal!'           
           if(nr.gt.0.and.res_atmname(res_atmnum(nr),nr)(1:2).eq.'HT')
     &             then
C last residue was the charged N-terminal residue ,change N and CA
               res_atmrad(1,nr)=1.65D0
               res_atmcharge(1,nr)=-0.3D0
               res_atmcharge(3,nr)=0.2D0
               res_charge(nr)=res_charge(nr)+1.0D0
           end if

           if(nr.gt.0.and.res_atmname(res_atmnum(nr),nr).eq.'OCT2')
     &             then
C last residue was the charged C-terminal residue, change C
               mol(1,nm)=nr
               res_atmrad((atomnum(res_indx)-1),nr)=1.87D0
               res_atmcharge((atomnum(res_indx)-1),nr)=0.14D0
               res_atmcharge(3,nr)=0.15D0
               res_atmname(atomnum(res_indx),nr)='OCT1'
               res_atmrad(atomnum(res_indx),nr)=1.4D0
               res_atmcharge(atomnum(res_indx),nr)=-0.57D0
               res_charge(nr)=res_charge(nr)-1.0D0
           end if
           if(endfile) goto 1001
c-----------------------------------------------------------------------

C Start a new residue
40         priv_res=line(18:27)
           nr=nr+1
           nsegr=nsegr+1
           res_segid(nr)=line(73:76)
           res_segcnt(nr)=nsegr

C find out the if the new residue is in the rtf table
	   do 100 res_indx=1,PROTN
	      if(line(18:21).eq.am_acid(res_indx)) goto 200
100	   continue
	   if(res_indx.gt.PROTN)  stop 'unknown aa readin'
200	   res_id(nr)=res_indx
           res_name(nr)=line(18:21)
           res_charge(nr)=rcharge(res_indx)
           res_atmnum(nr)=atomnum(res_indx)
           do 400 i=1,atomnum(res_indx)
              res_atmrad(i,nr)=-9999.0D0
              res_atmcharge(i,nr)=0.0D0
              res_atmname(i,nr)=atomname(i,res_indx)
              do 300 j=1,3
                 res_atmcoord(i,nr,j)=-9999.0D0
300           continue
400        continue
	   na=0
           
	end if
	
C find out if current atom is in this residue
	do atm_indx=1,atomnum(res_indx)
	   if(line(14:17).eq.atomname(atm_indx,res_indx))goto 500
	end do

C if it wasn't find in the rtf, check for special case
	if(atm_indx.gt.atomnum(res_indx)) then
           if(line(14:17).eq.'OCT1') then
              atm_indx=atomnum(res_indx)
              goto 500
	   else if(line(14:17).eq.'OXT '.or.line(14:17).eq.'OCT2') then
	      res_atmnum(nr)=res_atmnum(nr)+1
              na=na+1
	      atm_num=atm_num+1
              res_atmname(res_atmnum(nr),nr)='OCT2'
	      res_atmrad(res_atmnum(nr),nr)=1.4D0
              res_atmcharge(res_atmnum(nr),nr)=-0.57D0
	      read(line,2) (res_atmcoord(res_atmnum(nr),nr,i),i=1,3)
	      goto 999
	   else if(line(14:17).eq.'HT1 '.and.res_name(nr).ne.'PRO') then
              na=na+1
	      atm_num=atm_num+1
              res_atmname(2,nr)=line(14:17)
              res_atmrad(2,nr)=0.6D0
              res_atmcharge(2,nr)=0.35D0
              read(line,2) (res_atmcoord(2,nr,i),i=1,3)
	      goto 999
           else if(line(14:17).eq.'HT2 '.or.line(14:17).eq.'HT3 '.or.
     &       (line(14:17).eq.'HT1 '.and.res_name(nr).eq.'PRO')) then
	      res_atmnum(nr)=res_atmnum(nr)+1
              na=na+1
	      atm_num=atm_num+1
              res_atmname(res_atmnum(nr),nr)=line(14:17)
              res_atmrad(res_atmnum(nr),nr)=0.6D0
              res_atmcharge(res_atmnum(nr),nr)=0.35D0
              read(line,2) (res_atmcoord(res_atmnum(nr),nr,i),i=1,3)
	      goto 999
           end if
	   write(22,*) 'Unknow atom:',line(14:17),'in residue ',nr
	   goto 999
	end if

500	na=na+1	
	atm_num=atm_num+1
        res_atmrad(atm_indx,nr)=atomrad(atm_indx,res_indx)
        res_atmcharge(atm_indx,nr)=charge(atm_indx,res_indx)
	read(line,2) (res_atmcoord(atm_indx,nr,i),i=1,3)
2	format(30x,3f8.3)

999   end do	

1000  if(.not.endfile) then
         endfile=.true.
         goto 10
      end if
1001  mol(2,nm)=nr

      RETURN
      END

c ************************************************************************
c  Compute the Surface Area of a Atom Set
c ************************************************************************

      SUBROUTINE acces(res_num,atm_num,res_id,res_atmnum,res_atmname,
     &           X,Y,Z,RAD,ATAREA,RSAREA,SCAREA,IACC)

c  Constant and variable
c  ATOM property: X,Y,Z,RAD,CUBE,SEQNO,AATYP,res_atmname,RADSQ
c  CONTACT:       INOV,TAG,ARCI,ARCF,DX,DY,D,DSQ
c  CUBE porperty: ITAB,NATM

	integer      NCUBE,NAC,ICT,NSATM, ATM_RES
	parameter    (ATM_RES = 20)
        parameter    (NCUBE=5000,NAC=5000)
        parameter    (NSATM=24000,ICT=5000)

      CHARACTER*4 res_atmname(*)
      INTEGER 	  res_num, atm_num,IACC
      INTEGER     res_id(*), res_atmnum(*)
      REAL	  X(*), Y(*), Z(*)
      REAL        RAD(*), RADSQ(NSATM), ATAREA(*)
      REAL	  RSAREA(*),SCAREA(*)
      INTEGER	  INOV(ICT), TAG(ICT)
      REAL	  ARCI(ICT), ARCF(ICT)
      REAL	  DX(ICT),DY(ICT),D(ICT),DSQ(ICT)
      INTEGER     ITAB(NCUBE), NATM(NAC,NCUBE), CUBE(NSATM)

      real  XMIN,YMIN,ZMIN,XMAX,YMAX,ZMAX
C**   NumberofSlices: 1/P, WaterRadii:1.40 

      real  P,RH2O
      XMIN= 9999.9
      YMIN= 9999.9
      ZMIN= 9999.9
      XMAX= -9999.9
      YMAX= -9999.9
      ZMAX= -9999.9
      P   = 0.01
      RH2O= 1.40
      PI  = ACOS(-1.0)
      PIX2= 2.0*PI
	
      if(atm_num.gt.NSATM)  stop 'INCREASE NSATM'
     
C** THE RADIUS OF AN ATOM SPHERE = ATOM RADIUS + PROBE RADIUS
      RMAX=0.0
      KARC=ICT
      DO NR=1,res_num
      I0=(NR-1)*ATM_RES
      DO NA=1,res_atmnum(NR)
	 I=I0+NA
	 IF(x(i).gt.-990..and.y(i).gt.-990..and.z(i).gt.-990.) THEN
            RAD(I)=RAD(I)+RH2O
            RADSQ(I)=RAD(I)**2
            IF(RAD(I).GT.RMAX)RMAX=RAD(I)
            IF(XMIN.GT.X(I))XMIN=X(I)
            IF(YMIN.GT.Y(I))YMIN=Y(I)
            IF(ZMIN.GT.Z(I))ZMIN=Z(I)
            IF(XMAX.LT.X(I))XMAX=X(I)
            IF(YMAX.LT.Y(I))YMAX=Y(I)
            IF(ZMAX.LT.Z(I))ZMAX=Z(I)
	 ENDIF
      ENDDO
      ENDDO
      DMAX=RMAX*2.
      IF(IACC.NE.0) RH2O=0.0

C** CUBICALS CONTAINING THE ATOMS ARE SETUP. THE DIMENSION OF AN EDGE EQUALS
C** THE RADIUS OF THE LARGEST ATOM SPHERE
C** THE CUBES HAVE A SINGLE INDEX
      IDIM=(XMAX-XMIN)/DMAX+1.
      IF(IDIM.LT.3)IDIM=3
      JIDIM=(YMAX-YMIN)/DMAX+1.
      IF(JIDIM.LT.3)JIDIM=3
      JIDIM=IDIM*JIDIM
      KJIDIM=(ZMAX-ZMIN)/DMAX+1.
      IF(KJIDIM.LT.3)KJIDIM=3
      KJIDIM=JIDIM*KJIDIM
      if(KJIDIM.gt.NCUBE) stop 'INCREASE NCUBE'

C** PREPARE UPTO NCUBE CUBES EACH CONTAINING UPTO NAC ATOMS. THE CUBE INDEX
C** IS KJI. THE ATOM INDEX FOR EACH CUBE IS IN ITAB
      DO L=1,NCUBE
         ITAB(L)=0
      ENDDO

      DO NR=1,res_num
      L0=(NR-1)*ATM_RES

      DO NA=1,res_atmnum(NR)
         L=L0+NA
	 IF(x(l).gt.-990..and.y(l).gt.-990..and.z(l).gt.-990.) THEN
            I=(X(L)-XMIN)/DMAX+1.
            J=(Y(L)-YMIN)/DMAX
            K=(Z(L)-ZMIN)/DMAX
            KJI=K*JIDIM+J*IDIM+I
            N=ITAB(KJI)+1
	    if(N.gt.NAC) stop 'INCREASE NAC'
            ITAB(KJI)=N
            NATM(N,KJI)=L
            CUBE(L)=KJI
	 ENDIF
      ENDDO
      ENDDO

C** PROCESS EACH ATOM
      DO 5 NR=1,res_num
      IR0=(NR-1)*ATM_RES
      DO 5 NA=1,res_atmnum(NR)
      IR=IR0+NA
      IF(x(IR).lt.-990..and.y(IR).lt.-990..and.z(IR).lt.-990.) GOTO 5
      KJI=CUBE(IR)
      IO=0
      AREA=0.
      XR=X(IR)
      YR=Y(IR)
      ZR=Z(IR)
      RR=RAD(IR)
      RRX2=RR*2.
      RRSQ=RADSQ(IR)

C** FIND THE 'MKJI' CUBES NEIGHBORING THE KJI CUBE
      DO 6 KK=1,3
      K=KK-2
      DO 6 JJ=1,3
      J=JJ-2
      DO 6 I=1,3
         MKJI=KJI+K*JIDIM+J*IDIM+I-2
         IF(MKJI.LT.1) GO TO 6
         IF(MKJI.GT.KJIDIM) GO TO 14
         NM=ITAB(MKJI)
         IF(NM.LT.1) GO TO 6
C** RECORD THE ATOMS IN INOV THAT NEIGHBOR ATOM IR
         DO M=1,NM
            IN=NATM(M,MKJI)
            IF(IN.NE.IR) THEN
               IO=IO+1
	       if(IO.gt.ICT) then
		  write(22,*) 'IO=', IO, 'ICT=', ICT
		  stop
	       endif
               DX(IO)=XR-X(IN)
               DY(IO)=YR-Y(IN)
               DSQ(IO)=DX(IO)**2+DY(IO)**2
               D(IO)=SQRT(DSQ(IO))
               INOV(IO)=IN
            ENDIF
         ENDDO
    6 CONTINUE   
	
   14 IF(IO.NE.0)GO TO 17
      AREA=PIX2*RRX2
      GO TO 18

   17 continue

C** Z RESOLUTION DETERMINED
      NZP=1./P+0.5
      ZRES=RRX2/NZP
      ZGRID=Z(IR)-RR-ZRES/2.

C** SECTION ATOM SPHERES PERPENDICULAR TO THE Z AXIS
      DO 9 I=1,NZP
         ZGRID=ZGRID+ZRES
C** FIND THE RADIUS OF THE CIRCLE OF INTERSECTION OF THE IR SPHERE
C** ON THE CURRENT Z-PLANE
         RSEC2R=RRSQ-(ZGRID-ZR)**2
         RSECR=SQRT(RSEC2R)
         DO K=1,KARC
            ARCI(K)=0.0
         ENDDO
         KARC=0
         DO 10 J=1,IO
            IN=INOV(J)
C** FIND RADIUS OF CIRCLE LOCUS
            RSEC2N=RADSQ(IN)-(ZGRID-Z(IN))**2
            IF(RSEC2N.LE.0.0)GO TO 10
            RSECN=SQRT(RSEC2N)
C** FIND INTERSECTIONS OF N.CIRCLES WITH IR CIRCLES IN SECTION
            IF(D(J).GE.RSECR+RSECN)GO TO 10
C** DO THE CIRCLES INTERSECT, OR IS ONE CIRCLE COMPLETELY INSIDE THE OTHER?
            B=RSECR-RSECN
            IF(D(J).GT.ABS(B))GO TO 20
            IF(B.LE.0.0) GO TO 9
            GO TO 10
C** IF THE CIRCLES INTERSECT, FIND THE POINTS OF INTERSECTION
   20       KARC=KARC+1
            ALPHA=ACOS((DSQ(J)+RSEC2R-RSEC2N)/(2.*D(J)*RSECR))
            BETA=ATAN2(DY(J),DX(J))+PI
            TI=BETA-ALPHA
            TF=BETA+ALPHA
            IF(TI.LT.0.0)TI=TI+PIX2
            IF(TF.GT.PIX2)TF=TF-PIX2
            ARCI(KARC)=TI
            IF(TF.LT.TI) THEN
               ARCF(KARC)=PIX2
               KARC=KARC+1
	    ENDIF
            ARCF(KARC)=TF
   10    CONTINUE

C** FIND THE ACCESSIBLE CONTACT SURFACE AREA FOR THE SPHERE IR ON
C** THIS SECTION
         IF(KARC.EQ.0) THEN
            ARCSUM=PIX2
         ELSE
            CALL SORTAG(ARCI(1),KARC,TAG)
C** CALCULATE THE ACCESSIBLE AREA
            ARCSUM=ARCI(1)
            T=ARCF(TAG(1))
            IF(KARC.NE.1) THEN
            DO K=2,KARC
               IF(T.LT.ARCI(K))ARCSUM=ARCSUM+ARCI(K)-T
               TT=ARCF(TAG(K))
               IF(TT.GT.T)T=TT
            ENDDO
	    ENDIF
            ARCSUM=ARCSUM+PIX2-T
         ENDIF

         PAREA=ARCSUM*ZRES
         AREA=AREA+PAREA
    9 CONTINUE

   18 B=AREA*(RR-RH2O)**2/RR
      ATAREA(IR)=B
    5 CONTINUE

C** ONLY ACCOUNTING THE SIDE CHAIN CONTACT SURFACE
      DO NR=1,res_num
         RSAREA(NR)=0.0
         SCAREA(NR)=0.0
	 IR0=(NR-1)*ATM_RES
	 DO IA=1,res_atmnum(NR)
	 IR=IR0+IA
	 RSAREA(NR)=RSAREA(NR)+ATAREA(IR)
         if(res_atmname(IR).ne.'N   '.and.res_atmname(IR).ne.'C   '
     &      .and.res_atmname(IR).ne.'O   '.and.res_atmname(IR).ne.
     &      'CA  '.and.res_atmname(IR).ne.'NT  '.and.res_atmname
     &      (IR).ne.'OXT ') 
     &      SCAREA(NR)=SCAREA(NR)+ATAREA(IR)
         ENDDO
      ENDDO
 
      RETURN
      END



c ************************************************************************

      SUBROUTINE SORTAG(A,N,TAG)
      INTEGER TAG,TG
      DIMENSION A(N),IU(16),IL(16),TAG(N)
      DO I=1,N
      TAG(I)=I
      ENDDO
      M=1
      I=1
      J=N
  5   IF(I.GE.J) GO TO 70
 10   K=I
      IJ=(J+I)/2
      T=A(IJ)
      IF(A(I).LE.T) GO TO 20
      A(IJ)= A(I)
      A(I)=T
      T=A(IJ)
      TG=TAG(IJ)
      TAG(IJ)=TAG(I)
      TAG(I)=TG
 20   L=J
      IF(A(J).GE.T) GO TO 40
      A(IJ)=A(J)
      A(J)=T
      T=A(IJ)
      TG=TAG(IJ)
      TAG(IJ)=TAG(J)
      TAG(J)=TG
      IF(A(I).LE.T) GO TO 40
      A(IJ)=A(I)
      A(I)=T
      T=A(IJ)
      TG=TAG(IJ)
      TAG(IJ)=TAG(I)
      TAG(I)=TG
      GO TO 40
 30   A(L)=A(K)
      A(K)=TT
      TG=TAG(L)
      TAG(L)=TAG(K)
      TAG(K)=TG
 40   L=L-1
      IF(A(L).GT.T) GO TO 40
      TT=A(L)
 50   K=K+1
      IF(A(K).LT.T) GO TO 50
      IF(K.LE.L) GO TO 30
      IF(L-I.LE.J-K) GO TO 60
      IL(M)=I
      IU(M)=L
      I=K
      M=M+1
      GO TO 80
 60   IL(M)=K
      IU(M)=J
      J=L
      M=M+1
      GO TO 80
 70   M=M-1
      IF(M.EQ.0) RETURN
      I=IL(M)
      J=IU(M)
 80   IF(J-I.GE.1) GO TO 10
      IF(I.EQ.1) GO TO 5
      I=I-1
 90   I=I+1
      IF(I.EQ.J) GO TO 70
      T=A(I+1)
      IF(A(I).LE.T) GO TO 90
      TG=TAG(I+1)
      K=I
 100  A(K+1)=A(K)
      TAG(K+1)=TAG(K)
      K=K-1
      IF(T.LT.A(K)) GO TO 100
      A(K+1)=T
      TAG(K+1)=TG
      GO TO 90
      END

C ***********************************************************************
C Parameters of 20 amino acids

      SUBROUTINE getrtf1

C =====================================================================
C   These arrays are used in the param common block
        character*4   am_acid(29)
        character*4   atomname(29,20), atomtype(29,20)
        integer       atomnum(29)
        real          atomrad(29,20), atomchg(29,20)
        integer       atomislv(29,20)
        common /param/ am_acid,atomnum,atomname,atomtype,atomrad,
     1                 atomchg,atomislv

      OPEN(UNIT=10,FILE=
     *	'rtf188.fmt',STATUS='old')
      READ(10,*)
      do i = 1, 29
	 READ(10,513) am_acid(i), atomnum(i)
         do j = 1,atomnum(i)
            READ(10,514) atomname(i,j), atomtype(i,j),atomislv(i,j),
     1                  atomchg(i,j),atomrad(i,j)
         enddo
      enddo
      close(10)
513   FORMAT(1X,A4,I2)
514   FORMAT(5X,A4,1X,A4,2x,i2,2x,f6.2,20x,f10.4)

      RETURN
      END


