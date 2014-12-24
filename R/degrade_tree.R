
# phy 		= phylogeny 
# psi 		= preservation rate of fossils
# rho 		= probability of sampling (including in tree) a species 
#				given that the species is extant
# max.time 	= This parameter describes a tree where all lineages are extinct:
#					there are no extant taxa. As such, this should be greater
#					than the total tree depth. 
#				Mostly, this is to be ignored. not sure why I included it.
# tol		= Parameter used to determine whether a taxon is extinct or extant
#				if difference between taxon age and max.time is <= tol, taxon is extant

simulateFossilSampling <- function(phy, psi, rho, max.time = NULL, tol=0.01){
	
	# psi is parameter of sampling rate
	rr <- cumsum(phy$edge.length)
	ffmat <- cbind(c(0,rr[1:(length(rr)-1)]), rr)
	nsamples <- rpois(1, max(rr)*psi);
	fossils <- runif(nsamples, 0, max(rr));
	
	
	phy <- NU.branching.times(phy, return.type = 'begin.end');
	phy <- recursiveOrder(phy, node = length(phy$tip.label) + 1);
	
	# the root node:
	rt <- length(phy$tip.label) + 1;
	
	# convert the fossils vector into a set of times, relative to root age.
	fcopy <- NULL;
	for (i in 1:nrow(phy$edge)){
		fset <- fossils[fossils > ffmat[i,1] & fossils <= ffmat[i,2]];
		if (length(fset) > 0){
			fset <- fset - ffmat[i,1];
			fcopy <- c(fcopy, fset + phy$begin[i]);
			
		}
	}
	
	

	########### now to simulate sampling of extant taxa:
	if (is.null(max.time)){
		max.time <- max(phy$end);
	}
	phy$isExtant <- (max.time - phy$end) <= tol;
	
	livingtips <- phy$edge[,2][phy$isExtant];
	keepcount <- sum(rbinom(length(livingtips), size=1, prob = rho));
	
	### Add a "fossil" occurrence in the present for each taxon to keep:
	if (keepcount > 0){
		sampledtips <- sample(livingtips, keepcount);
		for (i in 1:length(sampledtips)){
			zz <- which(phy$edge[,2] == sampledtips[i]);
			fossils <- c(fossils, ffmat[zz,2]);
		}
	}	
	

	### Now to fill out nd and nx vectors:
	#	nx = count of fossils on branch
	#	nd = count of fossils on branch + all descendants

	# number of samples on branch
	nx <- numeric(nrow(phy$edge));
	
	# number on branch and on all descendant branches
	nd <- numeric(nrow(phy$edge));
 
	for (i in 1:length(phy$node_order)){
		zz <- phy$node_order[i];
		node <- phy$edge[zz,2];
		nx[zz] <- sum(fossils > ffmat[zz,1] & fossils <= ffmat[zz,2]);
		if (node <= rt){
			nd[zz] <- nx[zz];
		}else{
			dset <- phy$edge[,2][phy$edge[,1] == node];
			nd[zz] <- nx[zz] + sum(nd[phy$edge[,2] %in% dset]);
		}
		
	}
	
	### shorten branches to last fossil occurrence 
	for (i in 1:nrow(phy$edge)){
		node <- phy$edge[i,2];
		if (nx[i] > 0){
			fset <- fossils[fossils > ffmat[i,1] & fossils <= ffmat[i,2]];			
			deltaT <- ffmat[i,2] - max(fset)[1];
					
			if (node <= rt){
				# if tip, shorten to last fossil
				phy$edge.length[i]	<- phy$edge.length[i]  - deltaT;	
			}else{
				dset <- phy$edge[,2][phy$edge[,1] == node];
				if (sum(nd[which(phy$edge[,2] %in% dset)]) == 0){
					# if internal with fossils but NO fossil desc branches, shorten
					phy$edge.length[i] <- phy$edge.length[i]  - deltaT;
				}
			}
		}		
	}
	
	# set all branches with nd = 0 to zero length.
	phy$edge.length[nd == 0] <- 0;
	
	while (sum(phy$edge.length == 0) > 0){
		#cat(length(phy$tip.label), '\n')		
		
		badterminalnodes <- phy$edge[,2][phy$edge.length == 0];
		badterminalnodes <- badterminalnodes[badterminalnodes <= rt];
		to_drop <- phy$tip.label[badterminalnodes];
		
		phy <- drop.tip(phy, to_drop, trim.internal=F);

	}
	
	vNA <- which(phy$tip.label == 'NA');
	newnames <- paste('NN', 1:length(vNA), sep='');
	phy$tip.label[vNA] <- newnames;
	
	# should return a list where each element is the vector of relative times of 
	#	fossil occurrences on each branch. 
	#	Or at least a vector of fossil occurrence times, since the root, ignoring the 
	#	 ones for extant taxa
	
	obj <- list(edge=phy$edge, Nnode = phy$Nnode, tip.label = phy$tip.label, node.label=phy$node.label, edge.length = phy$edge.length, fossils = fcopy);
	class(obj) <- 'phylo';
	
	obj <- reorder(obj, order = 'cladewise');
	
	return(obj);
		
}



# Recursively compute branching times for phylogenetic tree.
#	Allows for non-ultrametric (fossil) trees.
#	Two return types:
#		return.type == 'bt'
#			returns traditional ape-like branching times (branching.times)
#		return.type == 'begin.end'
#			returns phylogeny with begin and end vectors
#				exactly like getStartStopTimes
#
#		This is slower than ape::branching.times, on account of
#			the recursion. Could recode in C for speed.
#
NU.branching.times <- function(phy, return.type = 'bt'){
	
	if (!is.binary.tree(phy)){
		stop("error. Need fully bifurcating (resolved) tree\n")
	}
	
	phy$begin <- rep(0, nrow(phy$edge)) 
	phy$end <-  rep(0, nrow(phy$edge))
	
	# Do it recursively
	
	fx <- function(phy, node){
	
		cur.time <- 0
		root <- length(phy$tip.label) + 1
		if (node > root){
			cur.time <- phy$end[which(phy$edge[,2] == node)]	
		}
	
		dset <- phy$edge[,2][phy$edge[,1] == node]
	
		i1 <- which(phy$edge[,2] == dset[1])
		i2 <- which(phy$edge[,2] == dset[2])
	
		phy$end[i1] <- cur.time + phy$edge.length[i1]
		phy$end[i2] <- cur.time + phy$edge.length[i2]

		if (dset[1] > length(phy$tip.label)){
			phy$begin[phy$edge[,1] == dset[1]] <- phy$end[i1]
			phy <- fx(phy, node = dset[1])	
			
		}
		if (dset[2] > length(phy$tip.label)){
			phy$begin[phy$edge[,1] == dset[2]] <- phy$end[i2]
			phy <- fx(phy, node = dset[2])
			
		}
		
		return(phy)
		
	}
	
	phy <- fx(phy, node = length(phy$tip.label) + 1)
	
	if (return.type == 'bt'){
		
		maxbt <- max(phy$end)
		nodes <- (length(phy$tip.label) + 1):(2*length(phy$tip.label) - 1)
		bt <- numeric(length(nodes))
		names(bt) <- nodes

		for (i in 1:length(bt)){
			tt <- phy$begin[phy$edge[,1] == nodes[i]][1]
			bt[i] <- maxbt - tt				
	
		}
		
		return(bt)
		
	}else if (return.type == 'begin.end'){
		return(phy)		
	}
	
}



recursiveOrder <- function(phy, node){
	
	nn <- which(phy$edge[,2] == node)
	
	if (node > length(phy$tip.label)){
		dset <- phy$edge[,2][phy$edge[,1] == node];
		phy <- recursiveOrder(phy, dset[1]);
		phy <- recursiveOrder(phy, dset[2]);			
	}
	
	if (is.null(phy$node_order)){
		phy$node_order <- nn;
	}else{
		phy$node_order <- c(phy$node_order, nn);
	}
	
	return(phy);
}	


# Allows mapping of event data from simulation on full tree
#	to match that generated by simulateFossilSampling
#	Object "events" must be the relevant event data as 
#	written by the BAMMtree simulation code

convertEventData <- function(truephy, paleophy, events, outname)
{
 
	root_age_true <- max(NU.branching.times(truephy));
	root_age_paleo <- max(NU.branching.times(paleophy));
	deltaT <- round(root_age_true - root_age_paleo, digits=5);
	dset <- truephy$tip.label;
	
	res <- NULL;
	
	for (i in 1:nrow(events)){
		
		nn <- getMRCA(truephy, tip = c(events$leftchild[i], events$rightchild[i]));
		if (is.null(nn)){
			if (events$leftchild[i] == events$rightchild[i]){
				nn <- which(truephy$tip.label == events$leftchild[i]);			
			}else{
				stop('Bad tips')
			}

		}
		
		iset <- NULL;
		if (nn > length(truephy$tip.label)){
			tmptree <- extract.clade(truephy, node = nn);	
			iset <- intersect(tmptree$tip.label, paleophy$tip.label);					
		}else {
			# Must be single:
			iset <- intersect(truephy$tip.label[nn], paleophy$tip.label);
		}
		


		if (length(iset) == 0){
			# Event gets dropped.	
		}else if (length(iset) == 1){
			tmp <- makeRowDF(1, iset, "NA", events$abstime[i], events$lambdainit[i], events$lambdashift[i], events$muinit[i], 0.0);
			
		}else{
			span <- getSpanningTips(paleophy, node = getMRCA(paleophy, tip = iset));
 
			tmp <- makeRowDF(1, span[1], span[2], events$abstime[i], events$lambdainit[i], events$lambdashift[i], events$muinit[i], 0.0);
		}
		
		if (length(iset) > 0){
			if (is.null(res)){
				res <- tmp;
			}else{
				res <- rbind(res, tmp);
			}			
		}

		
	}
	
 	res$abstime <- res$abstime - deltaT;	
 	
 	# Convert the root event: 
 	# get new lambda0:
 	rmin <- which(res$abstime == min(res$abstime));
 	lam0 <- res$lambdainit[rmin] * exp(res$lambdashift[rmin] * deltaT);
 	mu0 <- res$muinit[rmin] * exp(res$mushift[rmin] * deltaT);
 	
 	res$lambdainit[rmin] <- lam0;
 	res$muinit[rmin] <- mu0;
 	res$abstime[rmin] <- 0;

 	return(res);
 
}
 

makeRowDF <- function(g, lc, rc, at, li, ls, mi, ms){
	dff <- data.frame(generation=g, leftchild=lc, 
			rightchild=rc, abstime=at, lambdainit=li, lambdashift=ls, muinit=mi, mushift=ms , stringsAsFactors=F);
	return(dff);
}
 

# getSpanningTips
#	returns a pair of tips that span a given node. 
#	if the node is terminal, includes "NA"
getSpanningTips <- function(phy, node){
	
	if (node <= length(phy$tip.label)){
		return(c(phy$tip.label[node], 'NA'));
	}else{
		dnodes <- phy$edge[,2][phy$edge[,1] == node];
 
		
		while (dnodes[1] > length(phy$tip.label)){
			dnodes[1] <- phy$edge[,2][phy$edge[,1] == dnodes[1]][1];
		}
		while (dnodes[2] > length(phy$tip.label)){
			dnodes[2] <- phy$edge[,2][phy$edge[,1] == dnodes[2]][1];
		}		
	
		dset <- phy$tip.label[dnodes];
		return(dset);
	}
	
	
}






