c f77 rmsfit.f match.o -o rmsfit // by Christoph Champ 16-Dec-2004
c f77 rmsfit.f read.f match.f -o rmsfit

        integer      NRES,ATM_RES,PROTN,NMOL
        parameter    (NRES=1000,ATM_RES=20,PROTN=29,NMOL=10)

c rtf arrays
        character*4  am_acid(PROTN)
        character*4  atomname(ATM_RES,PROTN),atomtype(ATM_RES,PROTN)
        character*4  bond1(ATM_RES,PROTN),bond2(ATM_RES,PROTN)
        integer      atomnum(PROTN),bondnum(PROTN)
        double precision  charge(ATM_RES,PROTN),rcharge(PROTN)
        double precision  atomrad(ATM_RES,PROTN)
        common /param1/ am_acid,atomname,atomtype,bond1,bond2
        common /param2/ atomnum,bondnum,charge,rcharge,atomrad

C pdb 1
        integer       rnum1,nm1,anum1,r1id(NRES)
        character*4   r1name(NRES)
        character*4   r1aname(ATM_RES,NRES)
        integer       r1anum(NRES)
        double precision r1acoord(ATM_RES,NRES,3)
        double precision r1arad(ATM_RES,NRES)
        double precision r1charge(NRES),r1acharge(ATM_RES,NRES)
        character*4   r1segid(NRES)
        integer       r1segcnt(NRES)
        integer       mol1(2,NMOL)
        character*4   segid1(NMOL)

C pdb 2
        integer       rnum2,nm2,anum2,r2id(NRES)
        character*4   r2name(NRES)
        character*4   r2aname(ATM_RES,NRES)
        integer       r2anum(NRES)
        double precision r2acoord(ATM_RES,NRES,3)
        double precision r2arad(ATM_RES,NRES)
        double precision r2charge(NRES),r2acharge(ATM_RES,NRES)
        character*4   r2segid(NRES)
        integer       r2segcnt(NRES)
        integer       mol2(2,NMOL)
        character*4   segid2(NMOL)

C pdb 3 : the one to be moved
        character*80  line
        character*30  head(10000)
        character*26  tail(10000)

        character*80 ARG
        integer NARG, option
        double precision p(10000,3),q(10000,3),q2(10000,3),ss,ss2
        integer n,n2

        NARG=iargc()
        if(NARG.ne.5) then
           write(*,1) 
           write(*,2)
           write(*,3)
1          FORMAT("USAGE: rmsfit pdb1 pdb2 pdb3 pdbout option")
2          FORMAT("1->aa;2->Ca;3->bb")
3          FORMAT("rotate & translate pdb2 to match pdb1.")
           stop
        end if

        call getarg(1,ARG)
        open(unit=1,file=ARG,status='old')
        call getarg(2,ARG)
        open(unit=2,file=ARG,status='old')
        call getarg(3,ARG)
        open(unit=3,file=ARG,status='unknown')
        call getarg(4,ARG)
        open(unit=4,file=ARG,status='unknown')
        call getarg(5,ARG)
        read(ARG,'(i1)') option 

        call  getrtf(am_acid,atomname,atomtype,bond1,bond2,
     &              atomnum,bondnum,charge,rcharge,atomrad)

        call readin(1,rnum1,anum1,r1id,r1name,r1aname,r1anum
     &   ,r1acoord,r1arad,r1charge,r1acharge,r1segid,r1segcnt,
     &   nm1,mol1,segid1)

        call readin(2,rnum2,anum2,r2id,r2name,r2aname,r2anum
     &   ,r2acoord,r2arad,r2charge,r2acharge,r2segid,r2segcnt,
     &   nm2,mol2,segid2)

        if(rnum1.ne.rnum2) stop 'Diff residue number!'

        n2=1
10      do i=1,10000
           read(3,'(A80)',end=20) line
           if(line(1:4).ne.'ATOM') goto 10
           read(line,4) head(n2),(q2(n2,j),j=1,3),tail(n2)
4          FORMAT(A30,3F8.3,A24)
           n2=n2+1
        end do

20      n=1
        if(option.eq.1) then
           do i=1,rnum1
              do j=1,r1anum(i)
                 if(r1anum(i).ne.r2anum(i)) 
     &             stop 'Diff atom number!'
                 if(r1aname(j,i)(1:1).ne.'H') then
                    if(r1aname(j,i).ne.r2aname(j,i)) 
     &                 stop 'Diff atom name!'
                    do k=1,3
                      p(n,k)=r1acoord(j,i,k)
                      q(n,k)=r2acoord(j,i,k)
                    end do
                    n=n+1
                 end if
              end do
           end do
        else if(option.eq.2) then
           do i=1,rnum1
              if(r1id(i).le.26.and.r2id(i).le.26) then
                 do k=1,3
                   p(n,k)=r1acoord(3,i,k)
                   q(n,k)=r2acoord(3,i,k)
                 end do
                 n=n+1
              end if
           end do
        else if(option.eq.3) then
           do i=1,rnum1
              if(r1id(i).le.26.and.r2id(i).le.26) then
                 do k=1,3
                   p(n,k)=r1acoord(1,i,k)
                   q(n,k)=r2acoord(1,i,k)
                   p(n+1,k)=r1acoord(3,i,k)
                   q(n+1,k)=r2acoord(3,i,k)
                   p(n+2,k)=r1acoord(atomnum(r1id(i))-1,i,k)
                   q(n+2,k)=r2acoord(atomnum(r2id(i))-1,i,k)
                   p(n+3,k)=r1acoord(atomnum(r1id(i)),i,k)
                   q(n+3,k)=r2acoord(atomnum(r2id(i)),i,k)
                 end do
                 n=n+4
              end if
           end do
        end if

        call match(n-1,p,q,n2-1,q2,ss,ss2)

       do i=1,n2-1
         write(4,4) head(i),(q2(i,j),j=1,3),tail(i)
       end do
       write(4,'(A3)') 'END'

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


csubroutine reads a pdb file and store the coordinates to arrays
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
        parameter    (NRES=1000,ATM_RES=20,NMOL=10,PROTN=29)
  
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
	      if(line(18:20).eq.am_acid(res_indx)) goto 200
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

