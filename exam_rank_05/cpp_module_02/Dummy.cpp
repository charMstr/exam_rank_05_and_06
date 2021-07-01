/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dummy.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: charmstr <charmstr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/25 05:09:11 by charmstr          #+#    #+#             */
/*   Updated: 2021/06/25 05:10:09 by charmstr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Dummy.hpp"

//constructor
Dummy::Dummy(void) : ATarget("Target Practice Dummy")
{}

Dummy *
Dummy::clone(void) const
{
	return (new Dummy());	
}
